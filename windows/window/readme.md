Empty Window
============

###### How to compile

- First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi emptyWindow.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib
```

###### Preview

![emptyWindow](preview/emptyWindow.png "Empty Window")
