cd obj
g++ -Wall -std=c++17^
 -I..\nc^
 -I..\nc\debug^
 -I..\nc\engine^
 -I..\nc\graphics^
 -I..\nc\graphics\d3dresource^
 -I..\nc\graphics\DirectXMath\Inc^
 -I..\nc\graphics\primitive^
 -I..\nc\input^
 -I..\nc\math^
 -I..\nc\scene^
 -I..\nc\time^
 -I..\nc\utils\editor^
 -I..\nc\utils\editor\imgui^
 -I..\nc\utils\objloader^
 -I..\nc\win32^
 -I..\project\Components^
 -I..\project\Scenes^
 -c ..\nc\*.cpp^
 ..\nc\engine\*.cpp^
 ..\nc\input\*.cpp^
 ..\nc\scene\*.cpp^
 ..\nc\time\*.cpp^
 ..\nc\win32\*.cpp