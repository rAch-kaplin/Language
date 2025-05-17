#!/bin/bash

PROGRAM="../data/program.txt"
TREE="../data/tree_file.txt"
ASM="../data/file_asm.asm"

INPUT_FILE="../data/$2"

case $1 in
    frontend) ./bin/frontend -i "$INPUT_FILE" -o "$TREE" ;;
    backend)  ./bin/backend -i "$TREE" -o "$ASM" ;;
    asm)      ../CPU/build/bin/asm -f "$ASM" ;;
    proc)     ../CPU/build/bin/proc ;;
    all|"")   ./bin/frontend -i "$INPUT_FILE" -o "$TREE"
              ./bin/backend -i "$TREE" -o "$ASM"
              ../CPU/build/bin/asm -f "$ASM"
              ../CPU/build/bin/proc ;;
    *)        echo "Usage: $0 [frontend|backend|asm|proc|all]"
              echo "  no arguments = run all"
              exit 1 ;;
esac
