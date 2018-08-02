mkdir -p checkerboardTexture.app/Contents/MacOS

clang++ -o checkerboardTexture.app/Contents/MacOS/checkerboardTexture *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
