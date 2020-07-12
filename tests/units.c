#include <assert.h>
#include <sys/mman.h>

#include <criterion/criterion.h>

#include "../src/elf-loader.h"

Test(simple, read) {
    cr_assert(get_perms(PF_R) == PROT_READ);
}
