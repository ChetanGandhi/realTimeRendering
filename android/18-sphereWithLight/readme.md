# Sphere With Light

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
*   `Long tap` to enable or disable light.

##### Preview

*   Light Disabled
    ![lightDisabled][light-disabled-image]

*   Light Enabled
    ![lightEnabled][light-enabled-image]

[//]: # "Image declaration"
[light-disabled-image]: ./preview/lightDisabled.png "Light Disabled"
[light-enabled-image]: ./preview/lightEnabled.png "Light Enabled"
