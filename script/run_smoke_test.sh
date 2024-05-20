#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "usage: $0 <ICD_FILE_PATH> <INSTALL_DIR> <OUT_FILE>"
    exit 1
fi

SWIFTSHADER_INSTALL_DIR="$1"
ENGINE_INSTALL_DIR="$2"
OUT_FILE="$3"

echo "SWIFTSHADER_INSTALL_DIR: $SWIFTSHADER_INSTALL_DIR"
echo "ENGINE_INSTALL_DIR: $ENGINE_INSTALL_DIR"
echo "OUT_FILE: $OUT_FILE"

cd "$ENGINE_INSTALL_DIR/sample"

cat <<'EOF' > vk_swiftshader_driver.json
{
  "file_format_version": "1.0.0",
  "ICD": {
    "library_path": "./vk_swiftshader.dll",
    "api_version": "1.3"
  }
}
EOF

cp "$SWIFTSHADER_INSTALL_DIR/vk_swiftshader.dll" .
export VK_DRIVER_FILES="$ENGINE_INSTALL_DIR/sample/vk_swiftshader_driver.json"

echo "VK_DRIVER_FILES: " $VK_DRIVER_FILES
cat $VK_DRIVER_FILES

./Sample --run-test --config-path smoke_test_config.ini > "$OUT_FILE" 2>&1
EXIT_CODE=$?
echo $EXIT_CODE
exit $EXIT_CODE
