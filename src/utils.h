#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

size_t align(size_t size);

int roundUp(const int num, const int m);

int get_perms(const uint32_t flags);
