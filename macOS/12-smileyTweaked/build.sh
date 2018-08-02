mkdir -p smileyTweaked.app/Contents/MacOS

clang++ -o smileyTweaked.app/Contents/MacOS/smileyTweaked *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
