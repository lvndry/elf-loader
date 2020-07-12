#include <assert.h>
#include <elf.h>
#include <sys/mman.h>

#include <criterion/criterion.h>

#include "../../src/utils.h"

Test(perms, RONLY) {
    cr_assert(get_perms(PF_R) == PROT_READ);
}

Test(perm, RW) {
    cr_assert(get_perms(PF_R | PF_W) ==  PROT_READ | PROT_WRITE);
}

Test(perm, WRONLY) {
    cr_assert(get_perms(PF_W) ==  PROT_READ | PROT_WRITE);
}

Test(perm, XONLY) {
    cr_assert(get_perms(PF_X) ==  PROT_READ | PROT_EXEC);
}

Test(perm, XW) {
    cr_assert(get_perms(PF_X) ==  PROT_WRITE | PROT_EXEC);
}
