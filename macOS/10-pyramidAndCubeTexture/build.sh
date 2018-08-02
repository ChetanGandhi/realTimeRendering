mkdir -p pyramidAndCubeTexture.app/Contents/MacOS

clang++ -o pyramidAndCubeTexture.app/Contents/MacOS/pyramidAndCubeTexture *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
