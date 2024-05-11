#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "usage: $0 <ICD_FILE_PATH> <INSTALL_DIR> <OUT_FILE>"
    exit 1
fi

ICD_FILE_PATH="$1"
INSTALL_DIR="$2"
OUT_FILE="$3"

export VK_ICD_FILENAMES="$ICD_FILE_PATH"

echo "VK_ICD_FILENAMES: $VK_ICD_FILENAMES"
echo "INSTALL_DIR: $INSTALL_DIR"
echo "OUT_FILE: $OUT_FILE"

cd "$INSTALL_DIR/sample"
./Sample --run-test > "$OUT_FILE" 2>&1
EXIT_CODE=$?
echo $EXIT_CODE
echo $EXIT_CODE >> "$OUT_FILE"
exit $EXIT_CODE
