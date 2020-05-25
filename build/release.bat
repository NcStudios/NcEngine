cd obj
del *.*
g++ -O2 -std=c++17^
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
 -c ..\nc\*.cpp^
 ..\nc\component\*.cpp^
 ..\nc\engine\*.cpp^
 ..\nc\graphics\*.cpp^
 ..\nc\graphics\d3dresource\*.cpp^
 ..\nc\input\*.cpp^
 ..\nc\math\*.cpp^
 ..\nc\scene\*.cpp^
 ..\nc\time\*.cpp^
 ..\nc\utils\editor\imgui\*.cpp^
 ..\nc\utils\editor\*.cpp^
 ..\nc\utils\objloader\*.cpp^
 ..\nc\views\*.cpp^
 ..\nc\win32\*.cpp^
 ..\project\Components\*.cpp^
 ..\project\Scenes\*.cpp