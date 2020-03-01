cd obj
g++ -Wall -std=c++17^
 -I..\editor^
 -I..\external\DirectXMath\Inc^
 -I..\graphics^
 -I..\graphics\internal^
 -I..\graphics\primitive^
 -I..\win32^
 -c ..\graphics\*.cpp^
 ..\graphics\internal\*.cpp^
 ..\graphics\primitive\*.cpp