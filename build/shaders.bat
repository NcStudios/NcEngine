%~dp0\..\graphics\shader\fxc /T vs_5_0 /Fo %~dp0\..\graphics\shader\compiled\vertexshader.cso %~dp0\..\graphics\shader\VertexShader.hlsl
%~dp0\..\graphics\shader\fxc /T ps_5_0 /Fo %~dp0\..\graphics\shader\compiled\pixelshader.cso %~dp0\..\graphics\shader\PixelShader.hlsl
%~dp0\..\graphics\shader\fxc /T vs_5_0 /Fo %~dp0\..\graphics\shader\compiled\litvertexshader.cso %~dp0\..\graphics\shader\LitVertex.hlsl
%~dp0\..\graphics\shader\fxc /T ps_5_0 /Fo %~dp0\..\graphics\shader\compiled\litpixelshader.cso %~dp0\..\graphics\shader\LitPixel.hlsl