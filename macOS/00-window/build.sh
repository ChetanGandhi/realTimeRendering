mkdir -p window.app/Contents/MacOS

clang -o window.app/Contents/MacOS/window *.m -framework Cocoa
