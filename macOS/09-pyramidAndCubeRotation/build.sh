mkdir -p pyramidAndCubeRotation.app/Contents/MacOS

clang++ -o pyramidAndCubeRotation.app/Contents/MacOS/openGLWindow *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
