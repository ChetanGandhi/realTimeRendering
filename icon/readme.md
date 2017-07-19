Icon
====

###### How to compile

- First compile the resource file.

```
rc.exe resource\resource.rc
```

- Now compile the program with resource file.

- First compile the resource file.

```
rc.exe resource\resource.rc
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi icon.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib
```

###### Preview

![icon](preview/icon.png?raw=true "Icon")
