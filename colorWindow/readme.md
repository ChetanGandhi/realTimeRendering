Color Window
============

###### How to compile

- First compile the resource file.

```
cd resources
rc.exe /V resource.rc
cd ..
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi colorWindow.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib
```

###### Keyboard Shortcuts
- Press ```Esc``` key to quit.
- Press ```r``` key for red color background.
- Press ```g``` key for green color background.
- Press ```b``` key for blue color background.
- Press ```c``` key for cyan color background.
- Press ```m``` key for magenta color background.
- Press ```y``` key for yellow color background.
- Press ```k``` key for black color background.
- Press ```w``` key for white color background.
- Any other key will give error "No Color Assigned"

###### Preview
- Red Window
![redWindow][redWindow-image]

- Green Window
![greenWindow][greenWindow-image]

- Blue Window
![blueWindow][blueWindow-image]

- Cyan Window
![cyanWindow][cyanWindow-image]

- Magenta Window
![magentaWindow][magentaWindow-image]

- Yellow Window
![yellowWindow][yellowWindow-image]

- Black Window
![blackWindow][blackWindow-image]

- White Window
![whiteWindow][whiteWindow-image]

- No Color Assigned
![noColorWindow][noColorWindow-image]

<!-- Image declaration -->

[redWindow-image]: ./preview/red.png "Red Window"
[greenWindow-image]: ./preview/green.png "Green Window"
[blueWindow-image]: ./preview/blue.png "Blue Window"
[cyanWindow-image]: ./preview/cyan.png "Cyan Window"
[magentaWindow-image]: ./preview/magenta.png "Magenta Window"
[yellowWindow-image]: ./preview/yellow.png "Yellow Window"
[blackWindow-image]: ./preview/black.png "Black Window"
[whiteWindow-image]: ./preview/white.png "White Window"
[noColorWindow-image]: ./preview/no_color.png "No color assigned"
