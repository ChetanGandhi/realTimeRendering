Smiley Tweaked
==============

###### How to compile

- Compile the program.

```
g++ -o smiley smiley.cpp -lX11 -lGL -lGLU -lSOIL

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

<!-- Image declaration -->

[whiteSquare-image]: ./preview/whiteSquare.png "White Square"
[bottomLeftQuadrant-image]: ./preview/bottomLeftQuadrant.png "Bottom Left Quadrant"
[fullTexture-image]: ./preview/fullTexture.png "Full Texture"
[repeatTexture-image]: ./preview/repeatTexture.png "Repeat Texture"
[centerOfTexture-image]: ./preview/centerOfTexture.png "Center Of Texture"
