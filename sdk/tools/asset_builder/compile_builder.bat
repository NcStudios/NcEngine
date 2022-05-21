cd sdk/tools/asset_builder
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -I "%~dp0/../../external" -o "%~dp0/nc-builder" "%~dp0/AssetBuilder.cpp" -L"%~dp0/" -llibassimp