set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
cl.exe /EHsc /DUNICODE /Zi /I %FREEGLUT_PATH%\include solarSystem.cpp /link resources\resource.res /LIBPATH:%FREEGLUT_PATH%\lib\x64 user32.lib kernel32.lib gdi32.lib openGL32.lib freeglut.lib
