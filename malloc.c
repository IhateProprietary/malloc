#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <mach/mach_vm.h>
#include <mach/mach_init.h>
#include "malloc.h"

#if !defined(MAP_ANONYMOUS)
# define MAP_ANONYMOUS MAP_ANON
#endif

malloc_page_stat_t	pagestat;

malloc_page_t	*_alloc_page(size_t size)
{
	malloc_page_t	new_page;
	void			*page;

	new_page.prev_page = 0;
	new_page.request_pagesize = size;
	new_page.flags = 0;
	new_page.tail = size;
	new_page.last_free = 0;
	if (SIZE_NEEDS_ALIGN(size))
		new_page.size = MALLOC_ALIGN(size);
	else
		new_page.size = size + MALLOC_PRIVATE_ZONE;
	if ((page = mmap(0, new_page.size,
					 PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == MAP_FAILED)
		return ((malloc_page_t *)0);
	new_page.tail_zone = page + MALLOC_PRIVATE_ZONE;
	memcpy(page, &new_page, sizeof new_page);
	return (page);
}

void	*_init_page(size_t size, int pageflag, malloc_page_t **push)
{
	malloc_page_t	*new_page;
	malloc_page_t	*to_push;

	if ((new_page = _alloc_page(size)) == (malloc_page_t *)0)
		return ((void *)0);
	to_push = *push;
	new_page->flags = pageflag;
	new_page->next_page = to_push;
	if (to_push)
		to_push->prev_page = new_page;
	*push = new_page;
	pagestat.allocated += size;
	return ((void *)new_page);
}

void	*alloc_large(size_t size)
{
	malloc_page_t	*page;
	malloc_slice_t	*slice;
	void			*addr_to_return;

	if ((page =
		 _init_page(size, MALLOC_PAGE_LARGE, &pagestat.large)) == (void *)0)
		return ((void *)0);
	addr_to_return = page->tail_zone;
	slice = addr_to_return - MALLOC_SIZEOF_SLICE;
	slice->flags = MALLOC_SLICE_BUSY;
	slice->size = 0;
	slice->next_free = 0;
	slice->prev_free = 0;
	slice->page = 0;
	return (addr_to_return);
}

malloc_page_t	*malloc_healthy_page(malloc_page_t *pagelist, size_t size)
{
	malloc_page_t	*page;

	page = pagelist;
	while (page)
	{
		if ((uint32_t)page->tail >= size)
			break ;
		if ((uint32_t)page->max_free >= size && page->free_slice)
			break ;
		page = page->next_page;
	}
	if (page == (malloc_page_t *)0)
		page = _init_page(MALLOC_THRESHOLD, MALLOC_PAGE_SMALL, &pagestat.small);
	return (page);
}

malloc_slice_t	*_cherrypick_freeslice(void *base_addr, size_t size)
{
	malloc_slice_t		*slice;
	malloc_slice_t		*best_candidate;
	register uint32_t	next;

	slice = base_addr - MALLOC_SIZEOF_SLICE;
	best_candidate = slice;
	while (slice->next_free)
	{
		next = slice->size << 4;
		if (next >= size)
		{
			if (next == size)
				return (slice);
			if (next < best_candidate->size)
				best_candidate = slice;
		}
		next = slice->next_free << 4;
		base_addr += next;
		slice = base_addr - MALLOC_SIZEOF_SLICE;
	}
	if (best_candidate->size < size)
		best_candidate = (malloc_slice_t *)0;
	return (best_candidate);
}

malloc_slice_t	*cherrypick_slice(malloc_page_t *page, size_t size)
{
	malloc_slice_t	*slice;

	slice = (malloc_slice_t *)0;
	if (page->free_slice)
		slice = _cherrypick_freeslice(page->head_free, size);
	if (slice == (malloc_slice_t *)0 && (size_t)page->tail >= size)
	{
		slice = page->tail_zone - MALLOC_SIZEOF_SLICE;
		page->tail_zone += size;
	}
	return (slice);
}

malloc_slice_t	*malloc_slice(malloc_page_t *page, size_t size)
{
	malloc_slice_t	*pick;
	malloc_slice_t	*pn;
	void			*zone;
	int				flags;

	if ((pick = cherrypick_slice(page, size)) == (malloc_slice_t *)0)
		return ((malloc_slice_t *)0);
	flags = pick->flags;
	if (flags & MALLOC_SLICE_AVAILABLE)
	{
		zone = pick;
		zone += pick->prev_free << 4;
		pn = (malloc_slice_t *)zone;
		pn->next_free += pick->next_free;
		zone = pick;
		zone += pick->next_free << 4;
		pn = (malloc_slice_t *)zone;
		pn->prev_free += pick->prev_free;
		pick->flags ^= MALLOC_SLICE_AVAILABLE;
		pick->prev_free = 0;
		pick->next_free = 0;
	}
	else
	{
		pick->flags = MALLOC_SLICE_BUSY;
		pick->size = size >> 4;
		pick->page = ((uint64_t)pick - (uint64_t)page +
			MALLOC_SIZEOF_SLICE - MALLOC_PRIVATE_ZONE) >> 4;
	}
	return (pick);
}

void	*alloc_small(size_t size)
{
	malloc_page_t	*page;
	malloc_slice_t	*slice;
	void			*base_addr;
	void			*addr_to_return;

	size += MALLOC_SIZEOF_SLICE;
	if (size & MALLOC_ADDR_ALIGN)
		size = (size & ~MALLOC_ADDR_ALIGN) + 16;
	page = pagestat.small;
	while ((page = malloc_healthy_page(page, size)) != (malloc_page_t *)0)
	{
		if ((slice = malloc_slice(page, size)))
			break ;
		page = page->next_page;
	}
	if (page == (malloc_page_t *)0)
		return ((void *)0);
	page->busy_slice += 1;
	base_addr = slice;
	addr_to_return = base_addr + MALLOC_SIZEOF_SLICE;
	return (addr_to_return);
}

void 	*malloc(size_t size)
{
	void	*new_alloc;

	if (SIZE_EXCEEDED_THRESHOLD(size))
	{
		new_alloc = alloc_large(size);
	}
	else
	{
		new_alloc = alloc_small(size);
	}
	if (new_alloc == (void *)0)
	{
		errno = ENOMEM;
	}
	return new_alloc;
}

malloc_page_t	*check_slice_integrity(malloc_page_t *page, void *base_addr)
{
	malloc_slice_t	*slice;
	uint64_t		base;

	slice = base_addr - MALLOC_SIZEOF_SLICE;
	base = slice->page << 4;
	base_addr -= base + MALLOC_PRIVATE_ZONE;
	if ((malloc_page_t *)base_addr != page)
		return ((malloc_page_t *)0);
	return (page);
}

malloc_page_t	*check_addr_integrity(void *zone)
{
	malloc_page_t	*iter;
	void			*base_addr;

	iter = pagestat.small;
	while (iter)
	{
		base_addr = iter;
		if (zone > base_addr && zone < (base_addr + iter->true_pagesize))
			return (check_slice_integrity(iter, zone));
		if (base_addr > zone)
			break ;
		iter = iter->next_page;
	}
	iter = pagestat.large;
	while (iter)
	{
		base_addr = iter;
		if (zone > base_addr && zone < (base_addr + iter->true_pagesize))
			return (check_slice_integrity(iter, zone));
		iter = iter->next_page;
	}
	return ((malloc_page_t *)0);
}

void	munmap_page(malloc_page_t *page)
{
	malloc_page_t	*prev;
	malloc_page_t	*next;

	next = page->next_page;
	prev = page->prev_page;
	if (next)
		next->prev_page = prev;
	if (prev)
		prev->next_page = next;
	if (page.large == page)
		pagestat.large = next;
	else if (page.small == page)
		pagestat.small = next;
	munmap(page, page->true_pagesize);
}

void	free_small(malloc_page_t *page, void *ptr)
{
	malloc_slice_t	*slice;
	malloc_slice_t	*prev_free;
	void			*prev_ptr;

	slice = ptr - MALLOC_SIZEOF_SLICE;
	page->free_slice += 1;
	page->busy_slice -= 1;
	slice->flags |= MALLOC_SLICE_AVAILABLE;
	prev_ptr = page->free;
	if (prev_ptr)
	{
		prev_slice = prev_ptr - MALLOC_SIZEOF_SLICE;
		prev_slice->prev_free = (prev_ptr - ptr) >> 4;
		slice->next_free = (ptr - prev_ptr) >> 4;
	}
	page->free = ptr;
}

void	free(void *ptr)
{
	malloc_page_t	*page;

	if (ptr == (void *)0)
		return ;
	if ((page = check_addr_integrity(ptr)))
		return ; // abort
	if (page->flags & MALLOC_PAGE_LARGE)
	{
		munmap_page(page);
	}
	else
	{
		free_small(page, ptr);
		if (page->flags & MALLOC_PAGE_DEFRAG)
			free_defrag_page(page);
	}
}

void	*realloc_large(void *zone, size_t size)
{
	malloc_page_t	*page;
	malloc_slice_t	*slice;
	void			*new_addr;

	if ((page = check_addr_integrity(zone)) == (malloc_page_t *)0)
		return (zone);
	slice = zone - MALLOC_SIZEOF_SLICE;
	if ((new_addr = alloc_large(size)) == (void *)0)
		return (zone);
	memcpy(zone, new_addr, slice->size);
	// ->free zone;
	return (new_addr);
}

void	*realloc(void *ptr, size_t size)
{
	void	*realloc_zone;

	if (SIZE_EXCEEDED_THRESHOLD(size))
	{
		new_alloc = realloc_large(zone, size);
	}
	else
	{
		new_alloc = realloc_small(zone, size);
	}
	return (new_alloc);
}
