# Smiley Tweaked

###### Project creation and Setup

1.  Create a new Single View Application

    XCode > File > New > Project > iOS > Application > Single View Application

2.  Project Details

-   Product Name: Name of your project
-   Team: None
-   Organization Name: Keep empty
-   Organization Identifier: com.some.identifier
-   Language: Objective-C
-   Use Core Data: Un-check
-   Include Unit Tests: Un-check
-   Include UI Tests: Un-check

3.  Project settings

    In left panel > click on <project name> blue icon (.xCodeproj).
    In middle pane, from top tab bar, choose `General` tab.

-   Signing: Un-check `automatically manage signing`.
-   Keep `signing (debug)` and `signing (release)` untouched.
-   Deployment info:
-   Deployment Target: Select latest available option.
-   Device: iPhone and/or iPad.
-   Main Interface: Keep empty.
-   Device Orientation : Check landscape right and landscape left.
-   Check `Hide status bar` checkbox.
-   Check `Requires full screen` checkbox.
-   App icons and launch images
-   If you have icons, then click the arrow icon ahead of `app icons source` field and then drag and drop specific sized png images onto the image placeholders.
-   Launch screen file: Keep empty.

4.  Disable ARC (Automatic Reference Counting)

-   In left pane, click top level project name file (blue icon).
-   In middle pane, go to `Build Settings` and navigate to `Apple llvm - language - objective c`
    category.
-   Inside it look for `Objective-C Automatic Reference Counting` and select `No`.

5.  Resources

-   In left pane > right click on <project name> choose `New group`. Rename the `New group` to `resources`.
-   In left pane > click on `Issue Navigator` icon and then click on the actual issue text.
    From displayed dialog, choose `Add` to add the required default launch image for retina.
    Drag this image into this `resources` group.

###### Keyboard Shortcuts

-   Tap `1` show's bottom left quadrant of texture.
-   Tap `2` show's full texture.
-   Tap `3` show's repeat texture.
-   Tap `4` show's center of texture.
-   Tap `5` remove's the texture and show white square.

###### Preview

-   No Texture

    ![whiteSquare][white-square-image]

-   Bottom Left Quadrant of Texture

    ![bottomLeftQuadrant][bottom-left-quadrant-image]

-   Full Texture

    ![fullTexture][full-texture-image]

-   Repeating texture

    ![repeatTexture][repeat-texture-image]

-   Center of Texture

    ![centerOfTexture][center-of-texture-image]

[//]: # "Image declaration"
[white-square-image]: ./preview/whiteSquare.png "White Square"
[bottom-left-quadrant-image]: ./preview/bottomLeftQuadrant.png "Bottom Left Quadrant"
[full-texture-image]: ./preview/fullTexture.png "Full Texture"
[repeat-texture-image]: ./preview/repeatTexture.png "Repeat Texture"
[center-of-texture-image]: ./preview/centerOfTexture.png "Center Of Texture"
