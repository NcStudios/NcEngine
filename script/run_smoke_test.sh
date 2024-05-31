#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "usage: $0 <ENGINE_INSTALL_DIR> <OUT_FILE>"
    exit 1
fi

ENGINE_INSTALL_DIR="${1//\\//}"
OUT_FILE="$2"

echo "ENGINE_INSTALL_DIR: $ENGINE_INSTALL_DIR"
echo "OUT_FILE: $OUT_FILE"

cd "$ENGINE_INSTALL_DIR/sample"

./Sample --run-test --config-path smoke_test_config.ini > "$OUT_FILE" 2>&1
EXIT_CODE=$?
echo "smoke test exit code: $EXIT_CODE"
exit $EXIT_CODE
