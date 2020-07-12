TESTFILES += test-elfs.bats
BATH_PATH ?= ${}/bats/bin/
TESTBIN = ${BATH_PATH}bats

check: ${TESTFILES}
	${MAKE} -C elfs
	BATS_INSTALL_DIR=${BATS_INSTALL_DIR} TARGET=${TARGET} ${TESTBIN} $^

units:
	gcc -o units -lcriterion units.c ../src/elf-loader.c ../src/execute.S

clean:
	${MAKE} -C elfs clean && ${RM} *.o units

.PHONY: all test clean
