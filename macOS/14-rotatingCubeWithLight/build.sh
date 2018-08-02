mkdir -p rotatingCubeWithLight.app/Contents/MacOS

clang++ -o rotatingCubeWithLight.app/Contents/MacOS/rotatingCubeWithLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
