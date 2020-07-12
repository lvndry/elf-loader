#include <criterion/criterion.h>
#include <elf.h>

#include "../../src/utils.h"

Test(valid_auxv_type, platform) {
    cr_assert(is_valid_auxv(AT_PLATFORM) == 1);
}

Test(valid_auxv_type, execfd) {
    cr_assert(is_valid_auxv(AT_EXECFD) == 0);
}

Test(valid_auxv_type, minus) {
    cr_assert(is_valid_auxv(-1) == 0);
}
