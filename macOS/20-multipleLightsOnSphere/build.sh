mkdir -p multipleLightsOnSphere.app/Contents/MacOS

clang++ -o multipleLightsOnSphere.app/Contents/MacOS/multipleLightsOnSphere *.mm *.cpp -framework Cocoa -framework QuartzCore -framework OpenGL
