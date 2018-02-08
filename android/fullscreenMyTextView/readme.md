Fullscreen My Text View
=======================

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
![fullscreenMyTextView][fullscreenMyTextView-image]

[//]: # "Image declaration"

[fullscreenMyTextView-image]: ./preview/fullscreenMyTextView.png "Fullscreen My Text View"
