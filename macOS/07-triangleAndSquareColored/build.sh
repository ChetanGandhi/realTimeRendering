mkdir -p triangleAndSquareColored.app/Contents/MacOS

clang++ -o triangleAndSquareColored.app/Contents/MacOS/triangleAndSquareColored *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
