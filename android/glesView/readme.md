GLES View
=========

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
- GLES View
![glesView][glesView-image]

- Logcat output
![logcat][logcat-image]

<!-- Image declaration -->

[glesView-image]: ./preview/glesView.png "GLES View"
[logcat-image]: ./preview/logcat.png "Logcat output"
