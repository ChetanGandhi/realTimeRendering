mkdir -p triangleAndSquare.app/Contents/MacOS

clang++ -o triangleAndSquare.app/Contents/MacOS/triangleAndSquare *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
