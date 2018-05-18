set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
cl.exe /EHsc /DUNICODE /Zi /I %GLEW_PATH%\include /I %SPHERE_PATH% sphereWithLight.cpp /link resources\resource.res /LIBPATH:%GLEW32_LIB_PATH% /LIBPATH:%SPHERE_PATH% user32.lib kernel32.lib gdi32.lib openGL32.lib glew32.lib Sphere.lib
