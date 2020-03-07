%~dp0\..\nc\graphics\shader\fxc /T vs_5_0 /Fo %~dp0\..\nc\graphics\shader\compiled\vertexshader.cso %~dp0\..\nc\graphics\shader\VertexShader.hlsl
%~dp0\..\nc\graphics\shader\fxc /T ps_5_0 /Fo %~dp0\..\nc\graphics\shader\compiled\pixelshader.cso %~dp0\..\nc\graphics\shader\PixelShader.hlsl
%~dp0\..\nc\graphics\shader\fxc /T vs_5_0 /Fo %~dp0\..\nc\graphics\shader\compiled\litvertexshader.cso %~dp0\..\nc\graphics\shader\LitVertex.hlsl
%~dp0\..\nc\graphics\shader\fxc /T ps_5_0 /Fo %~dp0\..\nc\graphics\shader\compiled\litpixelshader.cso %~dp0\..\nc\graphics\shader\LitPixel.hlsl