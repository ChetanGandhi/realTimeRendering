# smileyTweaked

##### How to compile

```
gradlew.bat clean
gradlew.bat build
```

##### Start the adb server

```
adb start-server
```

##### List the devices

```
adb devices
```

##### Deploy the APK

```
adb -d install -r app\build\outputs\apk\debug\app-debug.apk
```

##### Run the application

-   Check the `AndroidManifest.xml` for required information.

```
adb shell am start -n com.package.name/.MainActivityClassName
```

##### Show logcat output

```
adb logcat -e "log to filter"
```

###### Update Gradle

-   In `gradle\wrapper\gradle-wrapper.properties` update the `distributionUrl`.

    You can get the latest version information from [Updating Gradle](https://developer.android.com/studio/releases/gradle-plugin#updating-gradle) page.

    ```
    distributionUrl = https\://services.gradle.org/distributions/gradle-version-all.zip
    ```

-   In `build.gradle` update the class path for gradle dependency.

    For more information check [Apply Plugin](https://developer.android.com/studio/build/gradle-plugin-3-0-0-migration#apply_plugin) page.

    ```
    buildscript {
        repositories {
            ...
            // You need to add the following repository to download the
            // new plugin.
            google()
        }

        dependencies {
            classpath 'com.android.tools.build:gradle:3.1.0'
        }
    }
    ```

###### Keyboard shortcuts

-   Swipe or scroll to quit.
-   1st single tap will show bottom left quadrant of texture.
-   2nd single tap will show full texture.
-   3rd single tap will repeat the texture 2 times.
-   4th single tap will show center of texture.
-   5th single tap will remove the texture and show white square.
-   Tap again to cycle through above mode.

###### Preview

-   No Texture
    ![whiteSquare][whitesquare-image]

-   Bottom Left Quadrant of Texture
    ![bottomLeftQuadrant][bottomleftquadrant-image]

-   Full Texture
    ![fullTexture][fulltexture-image]

-   Repeating texture
    ![repeatTexture][repeattexture-image]

-   Center of Texture
    ![centerOfTexture][centeroftexture-image]

[//]: # "Image declaration"
[whitesquare-image]: ./preview/whiteSquare.png "White Square"
[bottomleftquadrant-image]: ./preview/bottomLeftQuadrant.png "Bottom Left Quadrant"
[fulltexture-image]: ./preview/fullTexture.png "Full Texture"
[repeattexture-image]: ./preview/repeatTexture.png "Repeat Texture"
[centeroftexture-image]: ./preview/centerOfTexture.png "Center Of Texture"
