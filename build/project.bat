cd obj
g++ -Wall -std=c++17^
 -I..\nc^
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
 -I..\nc\win32^
 -I..\project\Components^
 -I..\project\Models^
 -I..\project\Scenes^
 -c ..\project\Components\*.cpp^
 ..\project\Scenes\*.cpp