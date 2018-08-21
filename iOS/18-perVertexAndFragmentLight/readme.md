# Per Vertex and Fragment Light

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

-   `Double tap` to switch between `per vertex` and `per fragment` light.
-   `Long tap` to toggle lighting.

###### Preview

-   Light Disabled

    ![lightDisabled][light-disabled-image]

-   Per Vertex Lighting Enabled

    ![perVertexLightEnabled][per-vertex-light-enabled-image]

-   Per Fragment Lighting Enabled

    ![perFragmentLightEnabled][per-fragment-light-enabled-image]

[//]: # "Image declaration"
[light-disabled-image]: ./preview/lightDisabled.png "Light Disabled"
[per-vertex-light-enabled-image]: ./preview/perVertexLightEnabled.png "Per Vertex Light Enabled"
[per-fragment-light-enabled-image]: ./preview/perFragmentLightEnabled.png "Per Fragment Light Enabled"
