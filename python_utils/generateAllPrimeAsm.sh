#!/bin/bash

SRC_DIR=../src
PYTHON=/usr/bin/python3
APP=asm_code_generator.py

genPrime() {
  # p = $1
  # a = $2
  # b = $3
  ${PYTHON} ${APP} --a $2 --b $3 > ${SRC_DIR}/P${1}/P${1}.S
}

set -x
genPrime 377 191 117
genPrime 434 216 137
genPrime 503 250 159
genPrime 546 273 172
genPrime 610 305 192
genPrime 697 356 215
genPrime 751 372 239
{ set +x; } 2>/dev/null
