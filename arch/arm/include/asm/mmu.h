#ifndef __ASM_MMU_H
#define __ASM_MMU_H

#include <asm/pgtable.h>
#include <malloc.h>
#include <errno.h>

#define PMD_SECT_DEF_UNCACHED (PMD_SECT_AP_WRITE | PMD_SECT_AP_READ | PMD_TYPE_SECT)
#define PMD_SECT_DEF_CACHED (PMD_SECT_WB | PMD_SECT_DEF_UNCACHED)

struct arm_memory;

static inline void mmu_enable(void)
{
}
void mmu_disable(void);
static inline void arm_create_section(unsigned long virt, unsigned long phys, int size_m,
		unsigned int flags)
{
}

static inline void setup_dma_coherent(unsigned long offset)
{
}

#ifdef CONFIG_MMU
void *dma_alloc_coherent(size_t size);
void dma_free_coherent(void *mem, size_t size);

void dma_clean_range(unsigned long, unsigned long);
void dma_flush_range(unsigned long, unsigned long);
void dma_inv_range(unsigned long, unsigned long);
unsigned long virt_to_phys(void *virt);
void *phys_to_virt(unsigned long phys);
void *map_io_sections(unsigned long physaddr, void *start, size_t size);

#else
static inline void *dma_alloc_coherent(size_t size)
{
	return xmemalign(4096, size);
}

static inline void dma_free_coherent(void *mem, size_t size)
{
	free(mem);
}

static inline void *phys_to_virt(unsigned long phys)
{
	return (void *)phys;
}

static inline unsigned long virt_to_phys(void *mem)
{
	return (unsigned long)mem;
}

static inline void dma_clean_range(unsigned long s, unsigned long e)
{
}

static inline void dma_flush_range(unsigned long s, unsigned long e)
{
}

static inline void dma_inv_range(unsigned long s, unsigned long e)
{
}

static inline void *map_io_sections(unsigned long phys, void *start, size_t size)
{
	return (void *)phys;
}

#endif

#ifdef CONFIG_CACHE_L2X0
void __init l2x0_init(void __iomem *base, __u32 aux_val, __u32 aux_mask);
#else
static inline void __init l2x0_init(void __iomem *base, __u32 aux_val, __u32 aux_mask)
{
}
#endif

struct outer_cache_fns {
	void (*inv_range)(unsigned long, unsigned long);
	void (*clean_range)(unsigned long, unsigned long);
	void (*flush_range)(unsigned long, unsigned long);
	void (*disable)(void);
};

extern struct outer_cache_fns outer_cache;

void __dma_clean_range(unsigned long, unsigned long);
void __dma_flush_range(unsigned long, unsigned long);
void __dma_inv_range(unsigned long, unsigned long);

#endif /* __ASM_MMU_H */

