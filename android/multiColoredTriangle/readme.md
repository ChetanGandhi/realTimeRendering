Multi Colored Triangle
======================

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

##### Preview
- Multi Colored Triangle
    ![multiColoredTriangle][multiColoredTriangle-image]

[//]: # "Image declaration"

[multiColoredTriangle-image]: ./preview/multiColoredTriangle.png "Multi Colored Triangle"
