#pragma once 
#include <stdint.h>
#include "memory/PMM/PageFrameAllocator.h"
#include "memory/VMM/VirtualMemoryManager.h"
#include "memory/Heap/Heap.h"
#include "memory/memory.h"
extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;