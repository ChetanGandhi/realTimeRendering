# Rotating Cube with Light

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
-   `Single tap` to enable or disable animation.
-   `Long tap` to enable or disable light.
-   `Double tap` to change rotation speed.

##### Preview

-   Light Disabled
    ![lightDisabled][light-disabled-image]

-   Light Enabled
    ![lightEnabled][light-enabled-image]

[//]: # "Image declaration"
[light-disabled-image]: ./preview/lightDisabled.png "Light Disabled"
[light-enabled-image]: ./preview/lightEnabled.png "Light Enabled"
