mkdir -p pyramidWithMultipleLight.app/Contents/MacOS

clang++ -o pyramidWithMultipleLight.app/Contents/MacOS/pyramidWithMultipleLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
