# Material Sphere

##### How to compile

-   This code is dependent on `Sphere.java` provided by Sir but not included along wih this source. You can change the implementation to draw any other shape.

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

-   Check the `AndroidManifest.xml` for required information.

```
adb shell am start -n com.package.name/.MainActivityClassName
```

##### Show logcat output

```
adb logcat -e "log to filter"
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

###### Keyboard shortcuts

-   `Swipe` or `Scroll` to quit.
-   `Single tap` to cycle through rotation axis and enable or disable animation.
-   `Long tap` to enable or disable light.
-   `Double tap` to change rotation speed.

##### Preview

-   Light Disabled

    ![lightsDisabled][lights-disabled-image]

-   Light Enabled

    ![lightsEnabled][lights-enabled-image]

-   Light Enabled - Rotation on X-Axis

    ![lightsEnabled][lights-enabled-x-axis-image]

-   Light Enabled - Rotation on Y-Axis

    ![lightsEnabled][lights-enabled-y-axis-image]

-   Light Enabled - Rotation on Z-Axis

    ![lightsEnabled][lights-enabled-z-axis-image]

[//]: # "Image declaration"
[lights-disabled-image]: ./preview/lightsDisabled.png "Lights Disabled"
[lights-enabled-image]: ./preview/lightsEnabled.png "Lights Enabled"
[lights-enabled-x-axis-image]: ./preview/lightsEnabledXAxis.png "Light Enabled - Rotation on X-Axis"
[lights-enabled-y-axis-image]: ./preview/lightsEnabledYAxis.png "Light Enabled - Rotation on Y-Axis"
[lights-enabled-z-axis-image]: ./preview/lightsEnabledZAxis.png "Light Enabled - Rotation on Z-Axis"
