#!/usr/bin/env bash
set -euo pipefail

# ====== config ======
BIN=${BIN:-./minishell}

# Résolution absolue pour s'isoler dans des tmpdirs
if [[ "$BIN" != /* ]]; then BIN_ABS="$(pwd)/$BIN"; else BIN_ABS="$BIN"; fi
if [ ! -x "$BIN_ABS" ]; then
  echo "Error: minishell not found at $BIN_ABS" >&2
  exit 1
fi

pass=0; fail=0
green(){ printf "\033[32m%s\033[0m\n" "$*"; }
red(){   printf "\033[31m%s\033[0m\n" "$*"; }

# Lance une session minishell dans un répertoire temporaire
run_session() {
  local label="$1"; shift
  local tmpdir tmpin tmpout tmpclean
  tmpdir="$(mktemp -d)"; tmpin="$(mktemp)"; tmpout="$(mktemp)"; tmpclean="$(mktemp)"
  cat >"$tmpin"
  ( cd "$tmpdir" && "$BIN_ABS" <"$tmpin" >"$tmpout" 2>&1 || true )
  # nettoie le bruit de debug éventuel
  sed -E \
    -e '/^minishell > /d' \
    -e '/^Tokens extraits/d' \
    -e '/^Token: /d' \
    -e '/^NODE:/d' \
    -e '/^CMD:/d' \
    -e '/^  REDIR:/d' \
    -e '/^exit$/d' \
    "$tmpout" > "$tmpclean"

  LAST_TMPDIR="$tmpdir"; LAST_OUT="$tmpout"; LAST_CLEAN="$tmpclean"
  echo "▶ $label"
}

# checkers
must_contain() {
  local s="$1"
  if grep -Fq -- "$s" "$LAST_CLEAN"; then green "  ✓ contains: $s"; pass=$((pass+1))
  else red "  ✗ missing: $s"; sed -n '1,120p' "$LAST_CLEAN" | sed 's/^/    /'; fail=$((fail+1)); fi
}
must_not_contain() {
  local s="$1"
  if grep -Fq -- "$s" "$LAST_CLEAN"; then red "  ✗ should not contain: $s"; sed -n '1,120p' "$LAST_CLEAN" | sed 's/^/    /'; fail=$((fail+1))
  else green "  ✓ not contains: $s"; pass=$((pass+1)); fi
}
must_match_line() { # exact line
  local s="$1"
  if grep -Fxq -- "$s" "$LAST_CLEAN"; then green "  ✓ line: $s"; pass=$((pass+1))
  else red "  ✗ line not found: $s"; sed -n '1,120p' "$LAST_CLEAN" | sed 's/^/    /'; fail=$((fail+1)); fi
}
file_has_lines() {
  local f="$1"; shift
  if [ ! -f "$LAST_TMPDIR/$f" ]; then red "  ✗ file missing: $f"; fail=$((fail+1)); return; fi
  local want
  for want in "$@"; do
    if ! grep -Fxq -- "$want" "$LAST_TMPDIR/$f"; then red "  ✗ file $f missing line: $want"; echo "    got:"; sed -n '1,120p' "$LAST_TMPDIR/$f" | sed 's/^/    /'; fail=$((fail+1)); return; fi
  done
  green "  ✓ file $f has expected lines"; pass=$((pass+1))
}
rc_is() { # RC:VAL exact
  local val="$1"
  if grep -Fxq -- "RC:$val" "$LAST_CLEAN"; then green "  ✓ RC:$val"; pass=$((pass+1))
  else red "  ✗ RC not $val"; sed -n '1,120p' "$LAST_CLEAN" | sed 's/^/    /'; fail=$((fail+1)); fi
}
rc_not_zero() {
  if grep -Eqx 'RC:[1-9][0-9]*' "$LAST_CLEAN"; then green "  ✓ RC non-zero"; pass=$((pass+1))
  else red "  ✗ RC not detected as non-zero"; sed -n '1,120p' "$LAST_CLEAN" | sed 's/^/    /'; fail=$((fail+1)); fi
}

########################
#        TESTS         #
########################

# 1) Pipeline: statut = dernier maillon
run_session "pipeline status last" <<'EOS'
false | true
echo "RC:$?"
true | false
echo "RC:$?"
exit
EOS
must_match_line "RC:0"
must_match_line "RC:1"

# 2) Group + pipeline + &&
run_session "group (pipe) && then echo" <<'EOS'
(echo a | tr a b) && echo c
exit
EOS
must_contain "b"
must_contain "c"

# 3) Group influence AND/OR
run_session "group AND/OR decisions from last of pipe" <<'EOS'
(false | true) && echo ok1
(true | false) && echo bad
(false | true) || echo bad2
(true | false) || echo ok2
exit
EOS
must_contain "ok1"
must_not_contain "bad"
must_not_contain "bad2"
must_contain "ok2"

# 4) Subshell n'altère pas l'env parent
run_session "subshell env isolation" <<'EOS'
export X=1
(export X=2)
echo "X=$X"
exit
EOS
must_match_line "X=1"

# 5) cd: pas en pipeline / en subshell
run_session "cd persist only in parent, not in subshell" <<'EOS'
pwd
mkdir d
(cd d)
pwd
cd d
pwd
cd ..
rmdir d
exit
EOS
# la ligne 2 pwd doit PAS se terminer par /d, la 3 oui. On teste juste présence "/d" une fois.
must_contain "/d"

# 6) Heredoc dans pipeline (décompte lignes)
run_session "heredoc | wc -l" <<'EOS'
cat << EOF | wc -l
L1
L2
EOF
exit
EOS
must_contain "2"

# 7) Heredoc expand / no-expand dans pipeline
run_session "heredoc expand/no-expand in pipe" <<'EOS'
export Z=WOW
cat << EOF | grep WOW
line-$Z
EOF
cat << 'EOF' | grep '\$Z'
line-$Z
EOF
exit
EOS
must_contain "line-WOW"
must_contain 'line-$Z'

# 8) Redirection: cible inexistante en input => RC != 0
run_session "< nofile -> non-zero RC" <<'EOS'
cat < __nope__
echo "RC:$?"
exit
EOS
rc_not_zero

# 9) Expansion de cible de redirection
run_session "redir target expands properly" <<'EOS'
export F=ttt.txt
echo HELLO > "$F"
cat "$F"
exit
EOS
file_has_lines "ttt.txt" "HELLO"
must_contain "HELLO"

# 10) PATH unset: commande nue échoue mais chemin absolu ok
run_session "unset PATH then absolute works" <<'EOS'
__SAVE="$PATH"
unset PATH
echo X || echo "RC:$?"
/bin/echo OK
export PATH="$__SAVE"
exit
EOS
must_contain "RC:"
must_contain "OK"

# 11) pipeline multi-maillons & rc
run_session "long pipeline rc from last" <<'EOS'
echo a | cat | wc -l
echo "RC:$?"
exit
EOS
must_contain "1"
rc_is 0

# 12) wildcard sans match reste littéral (si implémenté ainsi)
run_session "wildcard no match stays literal" <<'EOS'
echo xyz*
exit
EOS
must_contain "xyz*"

echo
echo "Summary: $pass passed, $fail failed"
if [ "$fail" -gt 0 ]; then exit 1; fi
