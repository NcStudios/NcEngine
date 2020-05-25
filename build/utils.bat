cd obj
g++ -Wall -std=c++17^
 -D NC_DEBUG^
 -I..\nc^
 -I..\nc\component^
 -I..\nc\debug^
 -I..\nc\engine^
 -I..\nc\graphics^
 -I..\nc\graphics\d3dresource^
 -I..\nc\graphics\DirectXMath\Inc^
 -I..\nc\input^
 -I..\nc\math^
 -I..\nc\scene^
 -I..\nc\time^
 -I..\nc\utils\editor^
 -I..\nc\utils\editor\imgui^
 -I..\nc\utils\objloader^
 -I..\nc\views^
 -I..\nc\win32^
 -I..\project\Components^
 -I..\project\Models^
 -I..\project\Scenes^
 -c ..\nc\utils\editor\*.cpp^
 ..\nc\utils\objloader\*.cpp