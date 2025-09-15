#!/usr/bin/env bash
set -euo pipefail

BIN=${BIN:-./minishell}

if [ ! -x "$BIN" ]; then
  echo "Error: minishell binary not found at $BIN" >&2
  exit 1
fi

pass=0
fail=0

run_case() {
  local label="$1"; shift
  local cmd="$1"; shift
  local tmpout
  tmpout="$(mktemp)"
  printf "%s\n" "$cmd" | "$BIN" >"$tmpout" 2>&1 || true

  local ok=1
  for pat in "$@"; do
    if ! grep -E "$pat" -q "$tmpout"; then
      echo "❌ FAIL: $label"
      echo "  Command : $cmd"
      echo "  Missing : $pat"
      echo "  Output  :"
      sed -n '1,120p' "$tmpout" | sed 's/^/    /'
      echo
      ok=0
      break
    fi
  done

  if [ "$ok" -eq 1 ]; then
    echo "✅ PASS: $label"
    pass=$((pass+1))
  else
    fail=$((fail+1))
  fi

  rm -f "$tmpout"
}

# ---- Basic words ----
run_case "words: ls -la" \
  "ls -la" \
  "Tokens extraits" \
  "Token:[[:space:]]*ls[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD[[:space:]]*\\|[[:space:]]*can_expand:[[:space:]]*true" \
  "Token:[[:space:]]*-la[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD[[:space:]]*\\|[[:space:]]*can_expand:[[:space:]]*true"

# ---- Quotes & expansion flags ----
run_case "quotes: single quotes block expansion" \
  "echo 'a b'" \
  "Token:[[:space:]]*echo[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD[[:space:]]*\\|[[:space:]]*can_expand:[[:space:]]*true" \
  "Token:[[:space:]]*'a b'[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD[[:space:]]*\\|[[:space:]]*can_expand:[[:space:]]*false"

run_case "quotes: double quotes allow expansion" \
  "echo \"\$HOME\"" \
  'Token:[[:space:]]*echo[[:space:]]*\|[[:space:]]*Type:[[:space:]]*WORD' \
  'Token:[[:space:]]*"\$HOME"[[:space:]]*\|[[:space:]]*Type:[[:space:]]*WORD[[:space:]]*\|[[:space:]]*can_expand:[[:space:]]*true'

# ---- Operators spacing & splitting ----
run_case "redir: no-space split" \
  "echo x>y" \
  "Token:[[:space:]]*echo[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD" \
  "Token:[[:space:]]*x[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD" \
  "Token:[[:space:]]*>[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(>|REDIR_OUT)" \
  "Token:[[:space:]]*y[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD"

run_case "append: >>" \
  "echo hi >> log" \
  "Token:[[:space:]]*>>[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(>>|APPEND)[[:space:]]*\\|[[:space:]]*can_expand:[[:space:]]*false" \
  "Token:[[:space:]]*log[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD[[:space:]]*\\|[[:space:]]*can_expand:[[:space:]]*true"

run_case "heredoc: << limiter" \
  "<< eof" \
  "Token:[[:space:]]*<<[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(<<|HEREDOC)[[:space:]]*\\|[[:space:]]*can_expand:[[:space:]]*false" \
  "Token:[[:space:]]*eof[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD[[:space:]]*\\|[[:space:]]*can_expand:[[:space:]]*true"

run_case "pipe/and/or/sep/parens" \
  "(a)&&b||c|d; e" \
  "Token:[[:space:]]*\\([[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(\\(|PAREN_OPEN)" \
  "Token:[[:space:]]*a[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD" \
  "Token:[[:space:]]*\\)[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(\\)|PAREN_CLOSE)" \
  "Token:[[:space:]]*&&[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(&&|AND)" \
  "Token:[[:space:]]*\\|\\|[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(\\|\\||OR)" \
  "Token:[[:space:]]*\\|[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(\\||PIPE)" \
  "Token:[[:space:]]*;[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(;|SEPARATOR)" \
  "Token:[[:space:]]*e[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD"

# ---- Wildcard token ----
run_case "wildcard: star token" \
  "echo *" \
  "Token:[[:space:]]*echo[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD" \
  "Token:[[:space:]]*\\*[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(\\*|WILDCARD|WORD)"

# espaces en tête/queue
run_case "spaces: leading/trailing" \
  "   echo   a   " \
  "Token:[[:space:]]*echo[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD" \
  "Token:[[:space:]]*a[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD"

# opérateurs collés
run_case "ops: && || compact" \
  "a&&b||c" \
  "Token:[[:space:]]*a[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD" \
  "Token:[[:space:]]*&&[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(&&|AND)" \
  "Token:[[:space:]]*b[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD" \
  "Token:[[:space:]]*\\|\\|[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(\\|\\||OR)" \
  "Token:[[:space:]]*c[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD"

# heredoc: délimiteur quoté -> can_expand=false
run_case "heredoc: quoted limiter disables expand" \
  "<< 'EOF'" \
  "Token:[[:space:]]*<<[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*(<<|HEREDOC)" \
  "Token:[[:space:]]*'EOF'[[:space:]]*\\|[[:space:]]*Type:[[:space:]]*WORD[[:space:]]*\\|[[:space:]]*can_expand:[[:space:]]*false"

# parenthèses imbriquées
run_case "parens: nested" \
  "((a))" \
  'Token:[[:space:]]*\([[:space:]]*\|[[:space:]]*Type:[[:space:]]*(\(|PAREN_OPEN)' \
  'Token:[[:space:]]*\([[:space:]]*\|[[:space:]]*Type:[[:space:]]*(\(|PAREN_OPEN)' \
  'Token:[[:space:]]*a[[:space:]]*\|[[:space:]]*Type:[[:space:]]*WORD' \
  'Token:[[:space:]]*\)[[:space:]]*\|[[:space:]]*Type:[[:space:]]*(\)|PAREN_CLOSE)' \
  'Token:[[:space:]]*\)[[:space:]]*\|[[:space:]]*Type:[[:space:]]*(\)|PAREN_CLOSE)'

# ---- Error handling (info only) ----
echo
echo "--- Unclosed quote behaviour (info) ---"
printf "echo 'oops\n" | "$BIN" | sed -n '1,40p'
printf "echo \"oops\n" | "$BIN" | sed -n '1,40p'

echo
echo "Summary: $pass passed, $fail failed"
if [ "$fail" -gt 0 ]; then
  exit 1
fi
