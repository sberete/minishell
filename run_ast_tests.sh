#!/usr/bin/env bash
set -euo pipefail

BIN=${BIN:-./minishell}

if [ ! -x "$BIN" ]; then
  echo "Error: minishell binary not found at $BIN" >&2
  exit 1
fi

run_case() {
  local label="$1"; shift
  local cmd="$1"; shift
  local tmpout
  tmpout="$(mktemp)"
  # feed the command then EOF
  printf "%s\n" "$cmd" | "$BIN" >"$tmpout" 2>&1 || true

  # Check all expected patterns
  local p
  for p in "$@"; do
    if ! grep -E "$p" -q "$tmpout"; then
      echo "❌ FAIL: $label"
      echo "  Command : $cmd"
      echo "  Missing : $p"
      echo "  Output  :"
      sed -n '1,120p' "$tmpout" | sed 's/^/    /'
      echo
      rm -f "$tmpout"
      return 1
    fi
  done

  echo "✅ PASS: $label"
  rm -f "$tmpout"
  return 0
}

all_pass=1

# ---------- Simple commands ----------
run_case "simple: word" \
  "d" \
  "NODE:\s*CMD" \
  "CMD:\s*d(\s|$)" || all_pass=0

run_case "simple: echo two args" \
  "echo test 123" \
  "NODE:\s*CMD" \
  "CMD:\s*echo\s+test\s+123" || all_pass=0

# ---------- Redirections ----------
run_case "redir: output > file" \
  "echo test > out.txt" \
  "NODE:\s*CMD" \
  "CMD:\s*echo\s+test" \
  "REDIR:\s*>\s+filename=out\.txt" || all_pass=0

run_case "redir: output append >> file" \
  "echo hi >> log" \
  "REDIR:\s*>>\s+filename=log" || all_pass=0

run_case "redir: input < file" \
  "cat < in.txt" \
  "REDIR:\s*<\s+filename=in\.txt" || all_pass=0

run_case "redir: heredoc << limiter" \
  "<< EOF" \
  "NODE:\s*CMD" \
  "REDIR:\s*<<\s+.*delim=EOF" || all_pass=0

run_case "redir: words and redirs mixed" \
  "cmd1 a < in b >> out" \
  "CMD:\s*cmd1\s+a\s+b" \
  "REDIR:\s*<\s+filename=in" \
  "REDIR:\s*>>\s+filename=out" || all_pass=0

# also check no-space redirs (lexer should split)
run_case "redir: no-space >" \
  "echo x>y" \
  "CMD:\s*echo\s+x" \
  "REDIR:\s*>\s+filename=y" || all_pass=0

# ---------- Pipes ----------
run_case "pipe: single" \
  "echo hi | wc -c" \
  "NODE:\s*PIPE" \
  "CMD:\s*echo\s+hi" \
  "CMD:\s*wc\s+-c" || all_pass=0

run_case "pipe: chain" \
  "printf aaaa | tr a b | tr b c" \
  "NODE:\s*PIPE" \
  "CMD:\s*printf\s+aaaa" \
  "CMD:\s*tr\s+a\s+b" \
  "CMD:\s*tr\s+b\s+c" || all_pass=0

# ---------- AND / OR ----------
run_case "and/or: simple AND" \
  "true && echo ok" \
  "NODE:\s*AND" \
  "CMD:\s*true" \
  "CMD:\s*echo\s+ok" || all_pass=0

run_case "and/or: simple OR" \
  "false || echo yep" \
  "NODE:\s*OR" \
  "CMD:\s*false" \
  "CMD:\s*echo\s+yep" || all_pass=0

# ---------- Sequence (;) ----------
run_case "sequence: two cmds" \
  "echo a ; echo b" \
  "NODE:\s*SEQ" \
  "CMD:\s*echo\s+a" \
  "CMD:\s*echo\s+b" || all_pass=0

# ---------- Grouping ----------
run_case "group: subshell" \
  "(echo a | tr a b) && echo c" \
  "NODE:\s*AND" \
  "NODE:\s*GROUP" \
  "NODE:\s*PIPE" \
  "CMD:\s*echo\s+a" \
  "CMD:\s*tr\s+a\s+b" \
  "CMD:\s*echo\s+c" || all_pass=0

# ---------- Edge / tricky ----------
run_case "redir: trailing after heredoc wordless cmd" \
  "<< eof" \
  "REDIR:\s*<<\s+.*delim=eof" || all_pass=0

run_case "redir: only redir with word target" \
  "> f" \
  "NODE:\s*CMD" \
  "REDIR:\s*>\s+filename=f" || all_pass=0

run_case "mix: args then redir then end" \
  "echo d > d" \
  "NODE:\s*CMD" \
  "CMD:\s*echo\s+d" \
  "REDIR:\s*>\s+filename=d" || all_pass=0

run_case "mix: arg, redir, arg" \
  "echo d > d d" \
  "CMD:\s*echo\s+d\s+d" \
  "REDIR:\s*>\s+filename=d" || all_pass=0

if [ "$all_pass" -eq 1 ]; then
  echo
  echo "🎉 All AST tests passed"
  exit 0
else
  echo
  echo "Some AST tests failed."
  exit 1
fi