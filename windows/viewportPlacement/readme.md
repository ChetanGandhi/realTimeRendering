Viewport Placement
==================

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
cl.exe /EHsc /DUNICODE /Zi viewportPlacement.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```0``` key to make viewport take full width and height.
- Press ```1``` key to place viewport at left bottom.
- Press ```2``` key to place viewport at right bottom.
- Press ```3``` key to place viewport at right top.
- Press ```4``` key to place viewport at left top.
- Press ```5``` key to place viewport at left half.
- Press ```6``` key to place viewport at right half.
- Press ```7``` key to place viewport at bottom half.
- Press ```8``` key to place viewport at top half.
- Press ```9``` key to place viewport at center.

###### Preview
* Viewport Placement Full
![viewportPlacementFull][viewportPlacementFull-image]

* Viewport Placement Left Bottom
![viewportPlacementLeftBottom][viewportPlacementLeftBottom-image]

* Viewport Placement Right Bottom
![viewportPlacementRightBottom][viewportPlacementRightBottom-image]

* Viewport Placement Right Top
![viewportPlacementRightTop][viewportPlacementRightTop-image]

* Viewport Placement Left Top
![viewportPlacementLeftTop][viewportPlacementLeftTop-image]

* Viewport Placement Left Half
![viewportPlacementLeftHalf][viewportPlacementLeftHalf-image]

* Viewport Placement Right Half
![viewportPlacementRightHalf][viewportPlacementRightHalf-image]

* Viewport Placement Bottom Half
![viewportPlacementBottomHalf][viewportPlacementBottomHalf-image]

* Viewport Placement Top Half
![viewportPlacementTopHalf][viewportPlacementTopHalf-image]

* Viewport Placement Center
![viewportPlacementCenter][viewportPlacementCenter-image]

[//]: # "Image declaration"

[viewportPlacementFull-image]: ./preview/viewportPlacementFull.png "OpenGl Viewport Placement Full"
[viewportPlacementLeftBottom-image]: ./preview/viewportPlacementLeftBottom.png "OpenGl Viewport Placement Left Bottom"
[viewportPlacementRightBottom-image]: ./preview/viewportPlacementRightBottom.png "OpenGl Viewport Placement Right Bottom"
[viewportPlacementRightTop-image]: ./preview/viewportPlacementRightTop.png "OpenGl Viewport Placement Right Top"
[viewportPlacementLeftTop-image]: ./preview/viewportPlacementLeftTop.png "OpenGl Viewport Placement Left Top"
[viewportPlacementLeftHalf-image]: ./preview/viewportPlacementLeftHalf.png "OpenGl Viewport Placement Left Half"
[viewportPlacementRightHalf-image]: ./preview/viewportPlacementRightHalf.png "OpenGl Viewport Placement Right Half"
[viewportPlacementBottomHalf-image]: ./preview/viewportPlacementBottomHalf.png "OpenGl Viewport Placement Bottom Half"
[viewportPlacementTopHalf-image]: ./preview/viewportPlacementTopHalf.png "OpenGl Viewport Placement Top Half"
[viewportPlacementCenter-image]: ./preview/viewportPlacementCenter.png "OpenGl Viewport Placement Center"
