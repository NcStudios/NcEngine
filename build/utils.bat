cd obj
g++ -Wall -std=c++17^
 -I..\external\DirectXMath\Inc^
 -I..\graphics^
 -I..\graphics\internal^
 -I..\nce^
 -I..\utils\editor^
 -I..\utils\editor\imgui^
 -I..\utils\objloader^
 -I..\win32^
 -c ..\utils\editor\*.cpp^
 ..\utils\editor\imgui\*.cpp^
 ..\utils\objloader\*.cpp