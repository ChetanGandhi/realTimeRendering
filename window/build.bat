cd resources
rc.exe /V resource.rc
cd ..
cl.exe /EHsc /DUNICODE /Zi emptyWindow.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib