#!/bin/sh

set -x

python tools/split_spelunky.py
prettier -w src/spelunky_a.js
prettier -w src/spelunky_b.js
python tools/patch.py
python tools/rollup.py > dist/index.js