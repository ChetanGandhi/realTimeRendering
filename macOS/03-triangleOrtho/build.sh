mkdir -p triangleOrtho.app/Contents/MacOS

clang++ -o triangleOrtho.app/Contents/MacOS/triangleOrtho *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
