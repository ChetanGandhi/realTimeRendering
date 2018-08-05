mkdir -p perVertexAndFragmentLight.app/Contents/MacOS

clang++ -o perVertexAndFragmentLight.app/Contents/MacOS/perVertexAndFragmentLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
