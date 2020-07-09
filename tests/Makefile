TESTFILES += test-elfs.bats
BATH_PATH ?= ${}/bats/bin/
TESTBIN = ${BATH_PATH}bats

check: ${TESTFILES}
	${MAKE} -C elfs
	BATS_INSTALL_DIR=${BATS_INSTALL_DIR} TARGET=${TARGET} ${TESTBIN} $^

clean:
	${MAKE} -C elfs clean

.PHONY: all test clean
