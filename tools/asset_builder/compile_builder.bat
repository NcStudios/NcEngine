cd tools/asset_builder
g++ -std=c++20 -I "%~dp0/../../nc/external/include" -o "%~dp0/build" "%~dp0/AssetBuilder.cpp" -L"%~dp0/" -llibassimp