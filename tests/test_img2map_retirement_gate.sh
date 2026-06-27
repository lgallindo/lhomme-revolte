#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

make -C tools/gif2map cli test >/dev/null

TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

mapfile -t LEVEL_GIFS < <(find assets -maxdepth 1 -type f -name 'level*.gif' | sort)

if [[ ${#LEVEL_GIFS[@]} -eq 0 ]]; then
  echo "No level GIFs found under assets/." >&2
  exit 1
fi

for gif in "${LEVEL_GIFS[@]}"; do
  base="$(basename "$gif" .gif)"

  tools/gif2map/build/gif2map --input "$gif" --format json --stdout > "$TMP_DIR/${base}.c.json"
  python3 assets/img2map.py --json "$gif" > "$TMP_DIR/${base}.py.json" 2> "$TMP_DIR/${base}.py.stderr"

  python3 - "$TMP_DIR/${base}.c.json" "$TMP_DIR/${base}.py.json" <<'PY'
import json
import sys

c_path, py_path = sys.argv[1], sys.argv[2]
c = json.load(open(c_path, encoding='utf-8'))
p = json.load(open(py_path, encoding='utf-8'))

if c != p:
    print(f"Mismatch between C and Python converter outputs: {c_path} vs {py_path}", file=sys.stderr)
    sys.exit(1)
PY

  tools/gif2map/build/gif2map --input "$gif" --format json --stdout > "$TMP_DIR/${base}.c2.json"
  cmp -s "$TMP_DIR/${base}.c.json" "$TMP_DIR/${base}.c2.json"
done

echo "img2map retirement gate passed (${#LEVEL_GIFS[@]} level GIF files)."
