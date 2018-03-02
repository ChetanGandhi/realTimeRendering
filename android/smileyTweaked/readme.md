smileyTweaked
======

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
* Check the ```AndroidManifest.xml``` for required information.
```
adb shell am start -n com.package.name/.MainActivityClassName
```

##### Show logcat output
```
adb logcat -e "log to filter"
```

###### Keyboard shortcuts
- Swipe or scroll to quit.
- 1st single tap will show bottom left quadrant of texture.
- 2nd single tap will show full texture.
- 3rd single tap will repeat the texture 2 times.
- 4th single tap will show center of texture.
- 5th single tap will remove the texture and show white square.
- Tap again to cycle through above mode.

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
