mkdir -p triangleAndSquareRotation.app/Contents/MacOS

clang++ -o triangleAndSquareRotation.app/Contents/MacOS/openGLWindow *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
