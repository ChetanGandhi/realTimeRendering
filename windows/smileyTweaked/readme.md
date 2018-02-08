Smiley Tweaked
==============

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
cl.exe /EHsc /DUNICODE /Zi smileyTweaked.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```1``` key to show bottom left quadrant of texture.
- Press ```2``` key to show full texture.
- Press ```3``` key to repeat the texture 2 times.
- Press ```4``` key to show center of texture.
- Press any other key to remove the texture and show white square.

###### Preview

- No Texture
![whiteSquare][whiteSquare-image]

- Bottom Left Quadrant of Texture
![bottomLeftQuadrant][bottomLeftQuadrant-image]

- Full Texture
![fullTexture][fullTexture-image]

- Repeating texture
![repeatTexture][repeatTexture-image]

- Center of Texture
![centerOfTexture][centerOfTexture-image]

[//]: # "Image declaration"

[whiteSquare-image]: ./preview/whiteSquare.png "White Square"
[bottomLeftQuadrant-image]: ./preview/bottomLeftQuadrant.png "Bottom Left Quadrant"
[fullTexture-image]: ./preview/fullTexture.png "Full Texture"
[repeatTexture-image]: ./preview/repeatTexture.png "Repeat Texture"
[centerOfTexture-image]: ./preview/centerOfTexture.png "Center Of Texture"
