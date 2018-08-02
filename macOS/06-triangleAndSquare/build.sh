mkdir -p openGLWindow.app/Contents/MacOS

clang++ -o openGLWindow.app/Contents/MacOS/openGLWindow *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
