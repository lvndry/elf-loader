run_cmp2 () {
  local e E T
  [[ ! "$-" =~ e ]] || e=1
  [[ ! "$-" =~ E ]] || E=1
  [[ ! "$-" =~ T ]] || T=1
  set +e
  set +E
  set +T
  local tgt ref
  local tgt_stdout tgt_stderr tgt_status
  local ref_stderr ref_stderr ref_status
  tgt="$1"
  ref="$2"
  shift 2
  tgt_stdout="$($tgt $ref "$@")"
  tgt_status="$?"
  tgt_stderr="$($tgt $ref "$@" 2>&1 > /dev/null)"
  ref_stdout="$($ref "$@")"
  ref_status="$?"
  ref_stderr="$($ref "$@" 2>&1 > /dev/null)"
  [ -z "$e" ] || set -e
  [ -z "$E" ] || set -E
  [ -z "$T" ] || set -T
  assert_equal_str "Check stdout" "$tgt_stdout" "$ref_stdout"
  assert_equal_str "Check exit status" "$tgt_status" "$ref_status"
}

run_cmp3 () {
  local e E T
  [[ ! "$-" =~ e ]] || e=1
  [[ ! "$-" =~ E ]] || E=1
  [[ ! "$-" =~ T ]] || T=1
  set +e
  set +E
  set +T
  local tgt ref
  local tgt_stdout tgt_stderr tgt_status
  local ref_stderr ref_stderr ref_status
  tgt="$1"
  ref="$2"
  shift 2
  tgt_stdout="$($tgt $ref "$@")"
  tgt_status="$?"
  tgt_stderr="$($tgt $ref "$@" 2>&1 > /dev/null)"
  ref_stdout="$($ref "$@")"
  ref_status="$?"
  ref_stderr="$($ref "$@" 2>&1 > /dev/null)"
  [ -z "$e" ] || set -e
  [ -z "$E" ] || set -E
  [ -z "$T" ] || set -T
  assert_equal_str "Check stdout" "$tgt_stdout" "$ref_stdout"
  assert_equal_str "Check stderr" "$tgt_stderr" "$ref_stderr"
  assert_equal_str "Check exit status" "$tgt_status" "$ref_status"
}

run_stderr () {
  local e E T oldIFS
  [[ ! "$-" =~ e ]] || e=1
  [[ ! "$-" =~ E ]] || E=1
  [[ ! "$-" =~ T ]] || T=1
  set +e
  set +E
  set +T
  stdout="$("$@" 2> /dev/null)"
  stderr="$("$@" 2>&1 > /dev/null)"
  status="$?"
  oldIFS=$IFS
  IFS=$'\n' stdout_lines=($stdout)
  IFS=$'\n' stderr_lines=($stderr)
  [ -z "$e" ] || set -e
  [ -z "$E" ] || set -E
  [ -z "$T" ] || set -T
  IFS=$oldIFS
}

assert_equal_str() {
  if [[ $2 != "$3" ]]; then
    batslib_print_kv_single_or_multi 8 \
        'expected' "$3" \
        'actual'   "$2" \
      | batslib_decorate "$1: values do not equal" \
      | fail
  fi
}
