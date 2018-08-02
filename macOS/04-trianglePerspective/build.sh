mkdir -p trianglePerspective.app/Contents/MacOS

clang++ -o trianglePerspective.app/Contents/MacOS/trianglePerspective *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
