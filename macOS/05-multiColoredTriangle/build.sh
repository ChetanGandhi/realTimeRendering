mkdir -p multiColoredTriangle.app/Contents/MacOS

clang++ -o multiColoredTriangle.app/Contents/MacOS/multiColoredTriangle *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
