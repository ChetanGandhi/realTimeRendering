Obj Parser
==========

This program reads a model file, i.e. obj file and shows how many triangles, vertices, texture and normals are there.

###### How to compile

```
cl.exe /EHsc /DUNICODE /Zi objParser.cpp /link user32.lib kernel32.lib gdi32.lib openGL32.lib
```

###### Output: When no mesh file is provided
```
Missing mesh file.
Usage:
        objParser.exe fileName.obj
```

###### Output: Mesh file data
```
--------------------
Mesh file: resources\models\monkeyHead.obj
Vertices: 507
Textures: 513
Normals: 941
Face triangles: 968
Face textures: 968
Face normals: 968
--------------------
```

###### Preview
![objParser][objParser-image]

[//]: # "Image declaration"

[objParser-image]: ./preview/objParser.png "Obj Parser"
