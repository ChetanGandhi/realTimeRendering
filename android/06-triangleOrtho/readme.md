Triangle by Orthographic Projection
===================================

###### How to compile

```
gradlew.bat clean
gradlew.bat build
```

###### Start the adb server

```
adb start-server
```

###### List the devices

```
adb devices
```

###### Deploy the APK

```
adb -d install -r app\build\outputs\apk\debug\app-debug.apk
```

###### Show Logcat output
```
adb logcat -e "log to filter"
```

###### Preview
- Triangle
    ![triangleOrtho][triangleOrtho-image]

[//]: # "Image declaration"

[triangleOrtho-image]: ./preview/triangleOrtho.png "Triangle by Orthographic Projection"
