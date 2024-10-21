#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <ENGINE_INSTALL_DIR>"
    exit 1
fi

ENGINE_INSTALL_DIR="${1//\\//}"
SMOKE_TEST_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

echo "ENGINE_INSTALL_DIR: $ENGINE_INSTALL_DIR"
echo "SMOKE_TEST_DIR: $SMOKE_TEST_DIR"

cd "$ENGINE_INSTALL_DIR/sample"

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ENGINE_INSTALL_DIR/sample"
fi

./Sample --run-test --config-path "$SMOKE_TEST_DIR/smoke_test_config.ini" --log-path "$ENGINE_INSTALL_DIR/SmokeTest.log"
EXIT_CODE=$?
echo "smoke test exit code: $EXIT_CODE"

if [ -s "ValidationLayers.log" ]; then
  echo "errors detected in ValidationLayers.log"
  exit 1
fi

exit $EXIT_CODE
