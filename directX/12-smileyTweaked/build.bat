set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%

cl.exe /EHsc /DUNICODE /Zi smileyTweaked.cpp /link /NODEFAULTLIB:msvcrt.lib resources\resource.res  /LIBPATH:".\lib\directXTK" user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib DirectXTK.lib
