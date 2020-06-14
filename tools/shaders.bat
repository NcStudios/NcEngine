fxc /T vs_5_0 /Fo %~dp0\..\project\shaders\compiled\vertexshader.cso %~dp0\..\project\shaders\VertexShader.hlsl
fxc /T ps_5_0 /Fo %~dp0\..\project\shaders\compiled\pixelshader.cso %~dp0\..\project\shaders\PixelShader.hlsl
fxc /T vs_5_0 /Fo %~dp0\..\project\shaders\compiled\litvertexshader.cso %~dp0\..\project\shaders\LitVertex.hlsl
fxc /T ps_5_0 /Fo %~dp0\..\project\shaders\compiled\litpixelshader.cso %~dp0\..\project\shaders\LitPixel.hlsl