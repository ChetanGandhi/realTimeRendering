Fullscreen Text View
====================

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
###### Preview
![fullscreenTextView][fullscreenTextView-image]

[//]: # "Image declaration"

[fullscreenTextView-image]: ./preview/fullscreenTextView.png "Fullscreen Text View"
