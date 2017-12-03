Material Sphere
===============

###### How to compile

- First compile the resource file.

```
set root=%CD%
cd resources
rc.exe /V resource.rc
cd %root%
```

- Now compile the program with resource file.

```
cl.exe /EHsc /DUNICODE /Zi materialSphere.cpp /link resources\resource.res user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Keyboard shortcuts
- Press ```Esc``` key to quit.
- Press ```f``` key to toggle fullscreen mode.
- Press ```l``` key to toggle lighting.
- Press ```x``` key to rotate light around x-axis.
- Press ```y``` key to rotate light around y-axis.
- Press ```z``` key to rotate light around z-axis.
- Press ```1``` to ```9``` to increase rotation speed.

###### Preview

- Lighting disabled
![materialSphereLightOff][materialSphereLightOff-image]

- Lighting enabled: X-Axis Rotation
![materialSphereLightOnXRotation-image][materialSphereLightOnXRotation-image]

- Lighting enabled: Y-Axis Rotation
![materialSphereLightOnYRotation-image][materialSphereLightOnYRotation-image]

- Lighting enabled: Z-Axis Rotation
![materialSphereLightOnZRotation-image][materialSphereLightOnZRotation-image]

<!-- Image declaration -->

[materialSphereLightOff-image]: ./preview/materialSphereLightOff.png "Material Sphere with light off"
[materialSphereLightOnXRotation-image]: ./preview/materialSphereLightOnXRotation.png "Material Sphere with light on and  X-Axis rotation"
[materialSphereLightOnYRotation-image]: ./preview/materialSphereLightOnYRotation.png "Material Sphere with light on and  Y-Axis rotation"
[materialSphereLightOnZRotation-image]: ./preview/materialSphereLightOnZRotation.png "Material Sphere with light on and  Z-Axis rotation"
