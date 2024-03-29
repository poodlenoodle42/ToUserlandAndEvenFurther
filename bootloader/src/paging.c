#include "paging.h"
#include <stdint.h>
#include <efilib.h>
typedef struct _PageStructureIndizes{
    uint64_t PDP_i;
    uint64_t PD_i;
    uint64_t PT_i;
    uint64_t P_i;
} PageStructureIndizes;

PageStructureIndizes getIndizes(uint64_t virtualAddr){
    PageStructureIndizes res;
    virtualAddr >>= 12;
    res.P_i = virtualAddr & 0x1ff;
    virtualAddr >>=9;
    res.PT_i = virtualAddr & 0x1ff;
    virtualAddr >>=9;
    res.PD_i = virtualAddr & 0x1ff;
    virtualAddr >>=9;
    res.PDP_i = virtualAddr & 0x1ff;
    return res;
}

uint64_t setaddr(uint64_t paddr){
    uint64_t value = 0;
    paddr &= 0x000000ffffffffff;
    value &= 0xfff0000000000fff;
    value |= (paddr << 12);
    return value + 0x3; //Set first and second bit, Present Flag
}
uint64_t getaddr(uint64_t entry){
    return (entry & 0x000ffffffffff000) >> 12;
}
typedef uint64_t PageDirEntry;
void MapMemory(void* PLM4, uint64_t paddr, uint64_t vaddr){
    PageStructureIndizes indizes = getIndizes(vaddr);
    PageDirEntry pde = ((PageDirEntry*)PLM4)[indizes.PDP_i];
    PageDirEntry* pagetable = NULL;
    if(pde == 0){
        ST->BootServices->AllocatePages(AllocateAnyPages,EfiLoaderData,1,(uint64_t*)&pagetable);
        ST->BootServices->SetMem((void*)pagetable,0x1000,0x00);
        ((PageDirEntry*)PLM4)[indizes.PDP_i] = setaddr(((uint64_t)pagetable) >> 12);
    } else {
        pagetable = (PageDirEntry*)(getaddr(pde) << 12);
    }
    pde = pagetable[indizes.PD_i];
    if(pde == 0){
        uint64_t newPage;
        ST->BootServices->AllocatePages(AllocateAnyPages,EfiLoaderData,1,&newPage);
        ST->BootServices->SetMem((void*)newPage,0x1000,0x00);
        ((PageDirEntry*)pagetable)[indizes.PD_i] = setaddr(newPage >> 12);
        pagetable = (PageDirEntry*)newPage;
    } else {
        pagetable = (PageDirEntry*)(getaddr(pde) << 12);
    }
    pde = pagetable[indizes.PT_i];
    if(pde == 0){
        uint64_t newPage;
        ST->BootServices->AllocatePages(AllocateAnyPages,EfiLoaderData,1,&newPage);
        ST->BootServices->SetMem((void*)newPage,0x1000,0x00);
        ((PageDirEntry*)pagetable)[indizes.PT_i] = setaddr(newPage >> 12);
        pagetable = (PageDirEntry*)newPage;
    } else {
        pagetable = (PageDirEntry*)(getaddr(pde) << 12);
    }
    pagetable[indizes.P_i] = setaddr(paddr >> 12);
}

void MapEfiMemoryMap(UINTN MapSize, UINTN DescriptorSize, EFI_MEMORY_DESCRIPTOR* Map, void* PLM4){
    UINTN map_ptr_value = (UINTN)Map;
	for(UINTN i = 0; i < MapSize / DescriptorSize; i++, map_ptr_value += DescriptorSize){
		Map = (EFI_MEMORY_DESCRIPTOR*)map_ptr_value;
		MapMemory(PLM4,Map->PhysicalStart,Map->PhysicalStart);
	}
}