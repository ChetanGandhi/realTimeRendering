# Material Sphere

##### How to compile

*   This code is dependent on `Sphere.java` provided by Sir but not included along wih this source. You can change the implementation to draw any other shape.

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

*   Check the `AndroidManifest.xml` for required information.

```
adb shell am start -n com.package.name/.MainActivityClassName
```

##### Show logcat output

```
adb logcat -e "log to filter"
```

###### Keyboard shortcuts

*   `Swipe` or `Scroll` to quit.
*   `Single tap` to cycle through rotation axis and enable or disable animation.
*   `Long tap` to enable or disable light.
*   `Double tap` to change rotation speed.

##### Preview

*   Light Disabled

    ![lightsDisabled][lights-disabled-image]

*   Light Enabled

    ![lightsEnabled][lights-enabled-image]

*   Light Enabled - Rotation on X-Axis

    ![lightsEnabled][lights-enabled-x-axis-image]

*   Light Enabled - Rotation on Y-Axis

    ![lightsEnabled][lights-enabled-y-axis-image]

*   Light Enabled - Rotation on Z-Axis

    ![lightsEnabled][lights-enabled-z-axis-image]

[//]: # "Image declaration"
[lights-disabled-image]: ./preview/lightsDisabled.png "Lights Disabled"
[lights-enabled-image]: ./preview/lightsEnabled.png "Lights Enabled"
[lights-enabled-x-axis-image]: ./preview/lightsEnabledXAxis.png "Light Enabled - Rotation on X-Axis"
[lights-enabled-y-axis-image]: ./preview/lightsEnabledYAxis.png "Light Enabled - Rotation on Y-Axis"
[lights-enabled-z-axis-image]: ./preview/lightsEnabledZAxis.png "Light Enabled - Rotation on Z-Axis"
