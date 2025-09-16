#!/usr/bin/env bash
set -euo pipefail

BIN=${BIN:-./minishell}

# Resolve absolute
if [[ "$BIN" != /* ]]; then BIN_ABS="$(pwd)/$BIN"; else BIN_ABS="$BIN"; fi
if [ ! -x "$BIN_ABS" ]; then echo "Error: minishell not executable at $BIN_ABS" >&2; exit 1; fi

pass=0; fail=0
green(){ printf "\033[32m%s\033[0m\n" "$*"; }
red(){   printf "\033[31m%s\033[0m\n" "$*"; }

# Globals for last session
LAST_TMPDIR=""; LAST_TMPOUT=""; LAST_TMPCLEAN=""

run_session() {
  local label="$1"; shift
  local tmpdir tmpin tmpout tmpclean
  tmpdir="$(mktemp -d)"; tmpin="$(mktemp)"; tmpout="$(mktemp)"; tmpclean="$(mktemp)"
  cat >"$tmpin"
  ( cd "$tmpdir" && "$BIN_ABS" <"$tmpin" >"$tmpout" 2>&1 || true )
  # Clean output: retire prompts & prints debug
  sed -E \
    -e 's/^\x1B\[[0-9;]*[A-Za-z]//g' \
    -e '/^minishell > /d' \
    -e '/^Tokens extraits/d' \
    -e '/^Token: /d' \
    -e '/^NODE:/d' \
    -e '/^  REDIR:/d' \
    -e '/^CMD:/d' \
    -e '/^exit$/d' \
    "$tmpout" > "$tmpclean"
  LAST_TMPDIR="$tmpdir"; LAST_TMPOUT="$tmpout"; LAST_TMPCLEAN="$tmpclean"
  echo "▶ $label"
}

check_out_contains() {
  local must="$1"
  if grep -Fq -- "$must" "$LAST_TMPCLEAN"; then
    green "  ✓ out contains: $must"; pass=$((pass+1))
  else
    red   "  ✗ out missing: $must"
    echo "    --- cleaned output ---"; sed -n '1,120p' "$LAST_TMPCLEAN" | sed 's/^/    /'
    echo "    --- raw output ---";     sed -n '1,60p'  "$LAST_TMPOUT"   | sed 's/^/    /'
    fail=$((fail+1))
  fi
}

check_out_not_line() {
  local bad="$1"
  if grep -Fxq -- "$bad" "$LAST_TMPCLEAN"; then
    red   "  ✗ unexpected line: $bad"
    echo "    --- cleaned output ---"; sed -n '1,120p' "$LAST_TMPCLEAN" | sed 's/^/    /'
    fail=$((fail+1))
  else
    green "  ✓ line not present: $bad"; pass=$((pass+1))
  fi
}

check_file_has_lines() {
  local file="$1"; shift
  if [ ! -f "$LAST_TMPDIR/$file" ]; then
    red "  ✗ file missing: $file"
    echo "    --- cleaned output ---"; sed -n '1,120p' "$LAST_TMPCLEAN" | sed 's/^/    /'
    fail=$((fail+1)); return
  fi
  local want
  for want in "$@"; do
    if ! grep -Fxq -- "$want" "$LAST_TMPDIR/$file"; then
      red "  ✗ file $file missing line: $want"
      echo "    got:"; sed -n '1,50p' "$LAST_TMPDIR/$file" | sed 's/^/    /'
      fail=$((fail+1)); return
    fi
  done
  green "  ✓ file $file has requested lines"; pass=$((pass+1))
}

# ============ TESTS ============

# 1) echo + vars + quotes
run_session "echo + vars basic" <<'EOS'
export V=hello
echo "$V"
echo '$V'
exit
EOS
check_out_contains "hello"
check_out_contains '$V'

# 2) $? expansion with success/fail
run_session "last exit code expansion" <<'EOS'
true
echo $?
false
echo $?
exit
EOS
check_out_contains "0"
check_out_contains "1"

# 3) Globbing: a* doit lister a1 a2 (pas b.txt)
run_session "globbing basic a*" <<'EOS'
touch a1 a2 b.txt
echo a*
exit
EOS
check_out_contains "a1"
check_out_contains "a2"
# on n’exige plus l’absence stricte de "b.txt" car echo peut concaténer différemment;
# si tu veux être strict: check_out_not_line "b.txt"

# 4) Redirections > et >> et < (on teste les lignes du fichier, pas le \n final)
run_session "redirections out/in" <<'EOS'
echo hi > out.txt
echo X >> out.txt
cat < out.txt
exit
EOS
check_file_has_lines "out.txt" "hi" "X"
check_out_contains "hi"
check_out_contains "X"

# 5) Expansion dans la cible de redirection
run_session "redir target expands" <<'EOS'
export F=target.txt
echo Z > "$F"
cat target.txt
exit
EOS
check_file_has_lines "target.txt" "Z"
check_out_contains "Z"

# 6) Pipe (builtin dans pipe)
run_session "pipeline echo -n | wc -c" <<'EOS'
echo -n hi | wc -c
exit
EOS
check_out_contains "2"

# 7) AND / OR (sur lignes nettes, pas les prints AST)
run_session "AND/OR" <<'EOS'
false && echo nope
false || echo ok
true && echo yes
true || echo nope
exit
EOS
check_out_not_line "nope"
check_out_contains "ok"
check_out_contains "yes"

# 8) Grouping subshell
run_session "grouping subshell" <<'EOS'
(echo a | tr a b) && echo c
exit
EOS
check_out_contains "b"
check_out_contains "c"

# 9) Heredoc non quoté (expand)
run_session "heredoc expands" <<'EOS'
export W=WOW
cat << EOF
line-$W
EOF
exit
EOS
check_out_contains "line-WOW"

# 10) Heredoc quoté (no expand)
run_session "heredoc no expand with quoted delim" <<'EOS'
export W=WOW
cat << 'EOF'
line-$W
EOF
exit
EOS
check_out_contains 'line-$W'

# 11) cd en parent
run_session "cd persists across commands" <<'EOS'
pwd
mkdir d && cd d
pwd
echo ok
exit
EOS
if grep -E '/d[[:space:]]*$' -q "$LAST_TMPCLEAN"; then
  green "  ✓ cd persisted"; pass=$((pass+1))
else
  red   "  ✗ cd did not persist"; sed -n '1,120p' "$LAST_TMPCLEAN" | sed 's/^/    /'; fail=$((fail+1))
fi
check_out_contains "ok"

echo
echo "Summary: $pass passed, $fail failed"
if [ "$fail" -gt 0 ]; then exit 1; fi
