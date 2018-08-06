# Fullscreen My Text View

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

###### Update Gradle

-   In `gradle\wrapper\gradle-wrapper.properties` update the `distributionUrl`.

    You can get the latest version information from [Updating Gradle](https://developer.android.com/studio/releases/gradle-plugin#updating-gradle) page.

    ```
    distributionUrl = https\://services.gradle.org/distributions/gradle-version-all.zip
    ```

-   In `build.gradle` update the class path for gradle dependency.

    For more information check [Apply Plugin](https://developer.android.com/studio/build/gradle-plugin-3-0-0-migration#apply_plugin) page.

    ```
    buildscript {
        repositories {
            ...
            // You need to add the following repository to download the
            // new plugin.
            google()
        }

        dependencies {
            classpath 'com.android.tools.build:gradle:3.1.0'
        }
    }
    ```

###### Preview

-   Hello World Gesture Detector
    ![helloWorldGestureDetector][helloworldgesturedetector-image]

-   Double Tap
    ![doubleTap][doubletap-image]

-   Single Tap Confirmed
    ![singleTapConfirmed][singletapconfirmed-image]

-   Fling
    ![fling][fling-image]

-   Long Press
    ![longPress][longpress-image]

-   Scroll
    ![scroll][scroll-image]

[//]: # "Image declaration"
[helloworldgesturedetector-image]: ./preview/helloWorldGestureDetector.png "Hello World Gesture Detector"
[doubletap-image]: ./preview/doubleTap.png "Double Tap"
[singletapconfirmed-image]: ./preview/singleTapConfirmed.png "Single Tap Confirmed"
[fling-image]: ./preview/fling.png "Fling"
[longpress-image]: ./preview/longPress.png "Long Press"
[scroll-image]: ./preview/scroll.png "Scroll"
