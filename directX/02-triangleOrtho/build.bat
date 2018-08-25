set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%

cl.exe /EHsc /DUNICODE /Zi triangleOrtho.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib
