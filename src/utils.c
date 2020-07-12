#include <elf.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "utils.h"

size_t align(size_t size)
{
    return size & ~(PAGE_SIZE -1);
}

// round to the nearest multiple of m
int roundUp(const int num, const int m)
{
    int remainder = num % m;
    if (remainder == 0)
        return num;
    return num + m - remainder;
}

int get_perms(const uint32_t flags)
{
    uint32_t prot = PROT_READ;

    if (flags & PF_W)
        prot |= PROT_WRITE;
    if (flags & PF_X)
        prot |= PROT_EXEC;

  return prot;
}
