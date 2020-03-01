cd obj
g++ -Wall -std=c++17^
 -I..\editor^
 -I..\editor\imgui^
 -I..\external\DirectXMath\Inc^
 -I..\graphics\internal^
 -I..\win32^
 -c ..\editor\*.cpp^
 ..\editor\imgui\*.cpp