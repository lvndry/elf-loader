#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

int get_perms(const uint32_t flags);
int is_valid_auxv(uint64_t type);
int roundUp(const int num, const int m);

size_t align(size_t size);
