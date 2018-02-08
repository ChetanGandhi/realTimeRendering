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

- Hello World Gesture Detector
![helloWorldGestureDetector][helloWorldGestureDetector-image]

- Double Tap
![doubleTap][doubleTap-image]

- Single Tap Confirmed
![singleTapConfirmed][singleTapConfirmed-image]

- Fling
![fling][fling-image]

- Long Press
![longPress][longPress-image]

- Scroll
![scroll][scroll-image]

[//]: # "Image declaration"

[helloWorldGestureDetector-image]: ./preview/helloWorldGestureDetector.png "Hello World Gesture Detector"
[doubleTap-image]: ./preview/doubleTap.png "Double Tap"
[singleTapConfirmed-image]: ./preview/singleTapConfirmed.png "Single Tap Confirmed"
[fling-image]: ./preview/fling.png "Fling"
[longPress-image]: ./preview/longPress.png "Long Press"
[scroll-image]: ./preview/scroll.png "Scroll"
