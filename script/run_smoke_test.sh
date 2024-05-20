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

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    SWIFTSHADER_LIB=vk_swiftshader.dll
elif [[ "$OSTYPE" == "linux-gnu" ]]; then
    SWIFTSHADER_LIB=libvk_swiftshader.so
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ENGINE_INSTALL_DIR/sample"
else
  echo "Unknown OS: $OSTYPE"
  exit 1
fi

cat <<EOF > vk_swiftshader_icd.json
{
  "file_format_version": "1.0.0",
  "ICD": {
    "library_path": "./$SWIFTSHADER_LIB",
    "api_version": "1.3"
  }
}
EOF

cp "$SWIFTSHADER_INSTALL_DIR/$SWIFTSHADER_LIB" .
export VK_DRIVER_FILES="$ENGINE_INSTALL_DIR/sample/vk_swiftshader_icd.json"

echo "VK_DRIVER_FILES: " $VK_DRIVER_FILES
echo "generated driver manifest:"
cat $VK_DRIVER_FILES

./Sample --run-test --config-path smoke_test_config.ini > "$OUT_FILE" 2>&1
EXIT_CODE=$?
echo "smoke test exit code: $EXIT_CODE"
exit $EXIT_CODE
