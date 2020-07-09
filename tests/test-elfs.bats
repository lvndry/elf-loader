load "$BATS_INSTALL_DIR/bats-support/load.bash"
load "$BATS_INSTALL_DIR/bats-assert/load.bash"
load "$BATS_TEST_DIRNAME/test-helper.bash"

@test "TRIVIAL: text section only ELF - must work even if segment is loaded at a bad adress" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/easy-exit"
	run_cmp2 "$TARGET" "$exe"
}

@test "TRIVIAL: ELF with text and rodata sections " {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/hello-world"
	run_cmp2 "$TARGET" "$exe"
}

@test "TRIVIAL: ELF with arguments - Check if argc is pushed on the stack - 1 argument" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/print-argc"
	run_cmp2 "$TARGET" "$exe"
}

@test "TRIVIAL: ELF with arguments - Check if argc is pushed on the stack - 12 arguments" {
	local exe exe_args
	exe="$BATS_TEST_DIRNAME/elfs/print-argc"
	exe_args="1 2 3 4 5 6 7 8 9 10 11"
	run_cmp2 "$TARGET" "$exe" $exe_args
}

@test "TRIVIAL: ELF with arguments - Check if argv is pushed on the stack - 1 argument" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/print-argv"
	run_cmp2 "$TARGET" "$exe"
}

@test "TRIVIAL: ELF with arguments - Check if argv is pushed on the stack - 12 argument" {
	local exe exe_args
	exe="$BATS_TEST_DIRNAME/elfs/print-argv"
	exe_args="1 2 3 4 5 6 7 8 9 10 11"
	run_cmp2 "$TARGET" "$exe" $exe_args
}

@test "TRIVIAL: ELF with arguments - Check if envp is pushed on the stack" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/print-envp"
	local e E T
	[[ ! "$-" =~ e ]] || e=1
	[[ ! "$-" =~ E ]] || E=1
	[[ ! "$-" =~ T ]] || T=1
	set +e
	set +E
	set +T
	local tgt_stdout tgt_stderr tgt_status
	local ref_stderr ref_stderr ref_status
	shift 2
	tgt_stdout="$($TARGET $exe "$@" | grep -v '^- TARGET=' | grep -v '^- _=')"
	tgt_status="$?"
	tgt_stderr="$($TARGET $exe "$@" 2>&1 > /dev/null)"
	ref_stdout="$($exe "$@" | grep -v '^- TARGET=' | grep -v '^- _=')"
	ref_status="$?"
	ref_stderr="$($exe "$@" 2>&1 > /dev/null)"
	[ -z "$e" ] || set -e
	[ -z "$E" ] || set -E
	[ -z "$T" ] || set -T
	assert_equal_str "Check stdout" "$tgt_stdout" "$ref_stdout"
	assert_equal_str "Check stderr" "$tgt_stderr" "$ref_stderr"
	assert_equal_str "Check exit status" "$tgt_status" "$ref_status"
}

@test "SIMPLE: ELF with a non trivial call to a function" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/helloworld"
	run_cmp2 "$TARGET" "$exe"
}

@test "SIMPLE: ELF with a call to a recursive function" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/fibo"
	run_cmp2 "$TARGET" "$exe" 13
}

@test "SIMPLE: Self check its data section content" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/data-check"
	run_cmp2 "$TARGET" "$exe"
}

@test "SIMPLE: Self check its rodata section content" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/rodata-check"
	run_cmp2 "$TARGET" "$exe"
}

@test "SIMPLE: Self check its bss section content" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/bss-check"
	run_cmp2 "$TARGET" "$exe"
}

@test "MEDIUM: Self check its segment permissions" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/segments-perms"
	run_cmp2 "$TARGET" "$exe"
}

@test "HARD: basename ELF" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/basename"
	run_cmp3 "$TARGET" "$exe" /looking/for/a/test
}

@test "HARD: uname ELF" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/uname"
	run_cmp3 "$TARGET" "$exe"
}

@test "HARD: head ELF" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/head"
	run_cmp3 "$TARGET" "$exe" -n 15 "$BATS_TEST_DIRNAME/elfs/LICENSE.ctools"
}

@test "HARD: falling head ELF" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/head"
	run_cmp3 "$TARGET" "$exe" -n 15 "toto"
}

@test "HARD: cat ELF" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/cat"
	run_cmp3 "$TARGET" "$exe" "$BATS_TEST_DIRNAME/elfs/README"
}

@test "HARD: failling cat ELF" {
	local exe
	exe="$BATS_TEST_DIRNAME/elfs/cat"
	run_cmp3 "$TARGET" "$exe" "toto"
}

# vim: ft=bash
