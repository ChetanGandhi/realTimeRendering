mkdir -p materialSphere.app/Contents/MacOS

clang++ -o materialSphere.app/Contents/MacOS/materialSphere *.mm *.cpp -framework Cocoa -framework QuartzCore -framework OpenGL
