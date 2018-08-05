mkdir -p perFragmentLight.app/Contents/MacOS

clang++ -o perFragmentLight.app/Contents/MacOS/perFragmentLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
