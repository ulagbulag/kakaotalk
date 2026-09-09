#!/bin/bash
# SPDX-License-Identifier: Unlicense
set -euo pipefail

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
test_dir="$(mktemp -d)"
trap 'rm -rf "${test_dir}"' EXIT

"${CC:-cc}" -Wall -Wextra -Werror -O2 -fPIC -shared \
    "${repo_dir}/input-guard.c" -o "${test_dir}/input-guard.so" -ldl
# Keep a real driver call frame so dladdr can identify Wine's event-loop caller.
"${CC:-cc}" -Wall -Wextra -Werror -O2 -fPIC -shared -fno-optimize-sibling-calls \
    "${repo_dir}/tests/wine-driver.c" -o "${test_dir}/winex11.so" -lX11
"${CC:-cc}" -Wall -Wextra -Werror -O2 \
    "${repo_dir}/tests/input-guard.c" -o "${test_dir}/input-guard-test" \
    -lX11 -lXi -lXtst -ldl

xvfb-run -a env LD_PRELOAD="${test_dir}/input-guard.so${LD_PRELOAD:+:${LD_PRELOAD}}" \
    "${test_dir}/input-guard-test" "${test_dir}/winex11.so"
