mkdir -p sphereWithLight.app/Contents/MacOS

clang++ -o sphereWithLight.app/Contents/MacOS/sphereWithLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
