mkdir -p perVertexLight.app/Contents/MacOS

clang++ -o perVertexLight.app/Contents/MacOS/perVertexLight *.mm -framework Cocoa -framework QuartzCore -framework OpenGL
