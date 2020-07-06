tools\fxc /T vs_5_0 /Fo %~dp0\..\project\shaders\compiled\vertexshader.cso %~dp0\..\project\shaders\VertexShader.hlsl
tools\fxc /T ps_5_0 /Fo %~dp0\..\project\shaders\compiled\pixelshader.cso %~dp0\..\project\shaders\PixelShader.hlsl
tools\fxc /T vs_5_0 /Fo %~dp0\..\project\shaders\compiled\litvertexshader.cso %~dp0\..\project\shaders\LitVertex.hlsl
tools\fxc /T ps_5_0 /Fo %~dp0\..\project\shaders\compiled\litpixelshader.cso %~dp0\..\project\shaders\LitPixel.hlsl
tools\fxc /T ps_5_0 /Fo %~dp0\..\project\shaders\compiled\pbrpixelshader.cso %~dp0\..\project\shaders\PBRPixelShader.hlsl
tools\fxc /T vs_5_0 /Fo %~dp0\..\project\shaders\compiled\pbrvertexshader.cso %~dp0\..\project\shaders\PBRVertexShader.hlsl