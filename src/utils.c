#include <elf.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "utils.h"
#include "stack.h"

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

int is_valid_auxv(uint64_t type)
{
    for (size_t i = 0; i < ARRAY_SIZE(auxv_fields); i++)
    {
        if (auxv_fields[i] == type)
        {
            return 1;
        }
    }
    return 0;
}
