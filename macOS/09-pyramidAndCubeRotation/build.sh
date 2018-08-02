mkdir -p pyramidAndCubeRotation.app/Contents/MacOS

clang++ -o pyramidAndCubeRotation.app/Contents/MacOS/pyramidAndCubeRotation *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
