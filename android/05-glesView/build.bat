adb start-server
adb devices

gradlew.bat clean && gradlew.bat build && adb -d install -r app\build\outputs\apk\debug\app-debug.apk
