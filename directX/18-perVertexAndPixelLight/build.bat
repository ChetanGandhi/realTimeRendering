set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%

cl.exe /EHsc /DUNICODE /Zi /I %SPHERE_PATH% perVertexAndPixelLight.cpp /link resources\resource.res /LIBPATH:%SPHERE_PATH% user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib Sphere.lib
