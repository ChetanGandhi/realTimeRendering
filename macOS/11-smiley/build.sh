mkdir -p smiley.app/Contents/MacOS

clang++ -o smiley.app/Contents/MacOS/smiley *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
