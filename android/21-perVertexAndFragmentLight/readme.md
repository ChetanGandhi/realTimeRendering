# Per Vertex and Fragment Light

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
-   `Long tap` to enable or disable light.
-   `Double tap` to switch between `per vertex` and `per fragment` light.

##### Preview

-   Light Disabled
    ![lightDisabled][light-disabled-image]

-   Per Vertex Light Enabled
    ![perVertexLightEnabled][per-vertex-light-enabled-image]

-   Per Fragment Light Enabled
    ![perFragmentLightEnabled][per-fragment-light-enabled-image]

[//]: # "Image declaration"
[light-disabled-image]: ./preview/lightDisabled.png "Light Disabled"
[per-vertex-light-enabled-image]: ./preview/perVertexLightEnabled.png "Per Vertex Light Enabled"
[per-fragment-light-enabled-image]: ./preview/perFragmentLightEnabled.png "Per Fragment Light Enabled"
