cd obj
g++ -Wall -std=c++17^
 -I..\external\DirectXMath\Inc^
 -I..\graphics^
 -I..\graphics\internal^
 -I..\graphics\primitive^
 -I..\nce^
 -I..\utils\editor^
 -I..\utils\editor\imgui^
 -I..\utils\objloader^
 -I..\win32^
 -c ..\graphics\*.cpp^
 ..\graphics\internal\*.cpp^
 ..\graphics\primitive\*.cpp