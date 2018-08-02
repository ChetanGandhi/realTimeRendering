mkdir -p triangleAndSquareRotation.app/Contents/MacOS

clang++ -o triangleAndSquareRotation.app/Contents/MacOS/triangleAndSquareRotation *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
