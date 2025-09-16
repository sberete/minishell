#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   chmod +x run_valgrind.sh
#   ./run_valgrind.sh
#   BIN=./build/minishell ./run_valgrind.sh
# Options:
#   STRICT=1  -> échoue aussi si ERROR SUMMARY != 0 (par défaut 0 = ignore)

BIN=${BIN:-./minishell}
STRICT=${STRICT:-0}

if [[ "$BIN" != /* ]]; then BIN_ABS="$(pwd)/$BIN"; else BIN_ABS="$BIN"; fi
if [ ! -x "$BIN_ABS" ]; then echo "Error: minishell not executable at $BIN_ABS" >&2; exit 1; fi

pass=0; fail=0
green(){ printf "\033[32m%s\033[0m\n" "$*"; }
red(){   printf "\033[31m%s\033[0m\n" "$*"; }

# en haut, près de BIN/STRICT
SUPP=${SUPP:-./valgrind_readline.supp}
VG_SUPP_OPTS=()
if [[ -n "$SUPP" && -f "$SUPP" ]]; then
  # calcule un chemin ABSOLU pour le passer à valgrind après cd "$tmpdir"
  if [[ "$SUPP" != /* ]]; then
    SUPP_ABS="$(pwd)/$SUPP"
  else
    SUPP_ABS="$SUPP"
  fi
  VG_SUPP_OPTS+=(--suppressions="$SUPP_ABS")
fi


run_vg() {
  local label="$1"; shift
  local tmpdir tmpin
  tmpdir="$(mktemp -d)"
  tmpin="$(mktemp)"
  cat >"$tmpin"

  (
    cd "$tmpdir" || exit 1
    : > out.txt
    valgrind \
      --tool=memcheck \
      --leak-check=full \
      --show-leak-kinds=all \
      --errors-for-leak-kinds=all \
      --track-origins=yes \
      --track-fds=yes \
      --error-exitcode=123 \
      --log-file=vg.log \
      "${VG_SUPP_OPTS[@]}" \
      "$BIN_ABS" < "$tmpin" >> out.txt 2>&1 || true
  )

  local vg="$tmpdir/vg.log"
  local out="$tmpdir/out.txt"
  [ -f "$out" ] || echo "<<no output captured>>" > "$out"
  [ -f "$vg"  ] || echo "<<no valgrind log>>"    > "$vg"

  local ok=1

  # (A) STRICT (optionnel) : check ERROR SUMMARY
  if [ "$STRICT" -eq 1 ]; then
    if grep -Eq 'ERROR SUMMARY: +0 errors' "$vg"; then
      green "  ✓ [$label] ERROR SUMMARY: 0 errors"
      pass=$((pass+1))
    else
      red   "  ✗ [$label] non-zero ERROR SUMMARY"
      sed -n '1,120p' "$vg" | sed 's/^/    /'
      fail=$((fail+1)); ok=0
    fi
  fi

  # (B) Fuites réelles (definitely lost) — extraction numérique robuste
  deflost="$(sed -n 's/.*definitely lost:[[:space:]]*\([0-9]\+\).*/\1/p' "$vg" | head -n1)"
  if [ -z "${deflost:-}" ]; then
    red "  ✗ [$label] unable to parse 'definitely lost' from valgrind log"
    sed -n '1,120p' "$vg" | sed 's/^/    /'
    fail=$((fail+1)); ok=0
  elif [ "$deflost" -eq 0 ]; then
    green "  ✓ [$label] no definite leaks"
    pass=$((pass+1))
  else
    red "  ✗ [$label] memory leaks (definitely lost = $deflost bytes)"
    # Montre le résumé des leaks
    grep -E 'definitely lost:|indirectly lost:|possibly lost:' "$vg" | sed 's/^/    /'
    fail=$((fail+1)); ok=0
  fi

  # (C) Invalid read/write hors readline/ncurses
  if grep -E 'Invalid (read|write)|Use of uninitialised|Conditional jump or move depends on uninitialised' "$vg" \
      | grep -Ev 'libreadline|libhistory|libtinfo|libncurses' >/dev/null 2>&1; then
    red "  ✗ [$label] invalid memory access (non-readline)"
    grep -E -n 'Invalid (read|write)|Use of uninitialised|Conditional jump or move depends on uninitialised' "$vg" \
      | grep -Ev 'libreadline|libhistory|libtinfo|libncurses' \
      | sed -n '1,40p' | sed 's/^/    /'
    fail=$((fail+1)); ok=0
  else
    green "  ✓ [$label] no invalid R/W (excluding readline)"
    pass=$((pass+1))
  fi

  # (D) FD ouverts à la fin
  if grep -q 'Open file descriptor' "$vg"; then
    red "  ✗ [$label] open file descriptors at exit"
    awk '/FILE DESCRIPTORS/{show=1} show{print} /END DESCRIPTORS/{show=0}' "$vg" | sed 's/^/    /' || true
    fail=$((fail+1)); ok=0
  else
    green "  ✓ [$label] no open file descriptors"
    pass=$((pass+1))
  fi

  echo "▶ $label"
  if [ "$ok" -eq 0 ]; then
    echo "  (See logs in: $tmpdir)"
  else
    rm -rf "$tmpdir" "$tmpin"
  fi
}

#####################################
#               TESTS               #
#####################################

run_vg "env churn" <<'EOS'
export A=1 B=2 C=3
unset B
export D=hello
unset A
export PATH="$PATH"
env | grep -E '^(C=3|D=hello)' >/dev/null
exit
EOS

run_vg "pipes & externals" <<'EOS'
echo a b c | tr ' ' '\n' | sort | wc -l
exit
EOS

run_vg "redirections + expansion" <<'EOS'
export F=out.txt
echo hi > "$F"
echo X >> "$F"
cat < "$F" > copy.txt
rm "$F"
cat copy.txt
exit
EOS

run_vg "globbing a*" <<'EOS'
touch a1 a2 b.txt
echo a*
rm -f a1 a2 b.txt
exit
EOS

run_vg "group and/or" <<'EOS'
(false && echo nope) || (echo ok)
(echo a | tr a b) && echo c
exit
EOS

run_vg "heredoc expand/noexpand" <<'EOS'
export W=WOW
cat << EOF
$W
EOF
cat << 'EOF'
$W
EOF
exit
EOS

run_vg "no PATH & not found" <<'EOS'
unset PATH
does_not_exist
exit
EOS

echo
echo "Summary: $pass passed, $fail failed"
if [ "$fail" -gt 0 ]; then exit 1; fi
