# Smiley Tweaked

###### Setup DirectXTK library

-   Download `DirectXTK` library source from [Microsodt's GitHub repository](https://github.com/Microsoft/DirectXTK).
    Either clone the repository or download the zip file. You can also get the release package.
-   Unzip it into a directory if you have downloaded the zip file.
-   You will find `.sln` or `.vcxproj` file for respective versions of Visual Studio.
-   Open respective `.sln` or `.vcxproj` file as per the Visual Studio version you have installed.
-   Once you open the `.sln` or `.vcxproj` file, you will find 3 projects in solution explorer like this
    -   `DirectXTK_Desktop_2017_Win10` (C++ Project)
    -   `MakeSpriteFont` (C# Project)
    -   `XWBTool_Desktop_2017` (C++ Project)
-   Select the build target as `Release`, then right click on `DirectXTK_Desktop_2017_Win10` and choose `rebuild` to build the `DirectXTK` library.

    Build either 32 bit or 64 bit version as per your machine configuration.

-   You will find the compiled library in `Bin` folder as per selected build configuration.
-   Copy the `DirectXTK.lib`, `DirectXTK.pdb` and `DirectXTK.pch` files to your DirectX project.
    The `DirectXTK.pdb` and `DirectXTK.pch` are only required if you have built a debug version.
-   Similarly copy the `WICTextureLoader.h` from the `Inc` folder to your DirectX project, next to the `DirectXTK.lib` file.
-   Add it to project by `Add Existing Item` method if you are using Visual Studio.
-   Include the `WICTextureLoader.h` header file in code.

    ```
    ...

    #include "WICTextureLoader.h"

    ...
    ```

-   **Important about command line building**

    If you are using debug version of `DirectXTK.lib` and if you get linking errors, then try using `release` version.
    This may raise `lnk4098` warning of default lib `msvcrt`.

    To resolve this issue, add `/nodefaultlib:msvcrt.lib` linker command line switch.

###### How to compile

-   First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

-   Now compile the program with resource file

```
cl.exe /EHsc /DUNICODE /Zi smileyTweaked.cpp /link /NODEFAULTLIB:msvcrt.lib resources\resource.res  /LIBPATH:".\lib\directXTK" user32.lib kernel32.lib gdi32.lib d3d11.lib D3dcompiler.lib DirectXTK.lib
```

###### Keyboard shortcuts

-   Press `Esc` key to quit.
-   Press `f` key to toggle fullscreen mode.
-   Press `1` key to show bottom left quadrant of texture.
-   Press `2` key to show full texture.
-   Press `3` key to repeat the texture 2 times.
-   Press `4` key to show center of texture.

###### Preview

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
