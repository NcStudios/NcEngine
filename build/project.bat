cd obj
g++ -Wall -std=c++17^
 -I..\editor^
 -I..\editor\imgui^
 -I..\external\DirectXMath\Inc^
 -I..\graphics\internal^
 -I..\graphics\primitive^
 -I..\nce^
 -I..\nce\debug^
 -I..\nce\input^
 -I..\nce\internal^
 -I..\nce\math^
 -I..\nce\scene^
 -I..\nce\time^
 -I..\project\Components^
 -I..\project\Scenes^
 -I..\win32^
 -c ..\project\Components\*.cpp^
 ..\project\Scenes\*.cpp