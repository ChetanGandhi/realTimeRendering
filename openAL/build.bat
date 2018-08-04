set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%

cl.exe /EHsc /DUNICODE /Zi /Fe:main.exe /I %OPENAL_PATH%\include *.cpp lib/*.cpp /link resources\resource.res /LIBPATH:%OPENAL_LIB_PATH% user32.lib kernel32.lib gdi32.lib OpenAL32.lib
