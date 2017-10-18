gcc -c -fPIC mathHelper.c -o mathHelper.o
gcc -shared -o libmathHelper.so mathHelper.o
gcc -o main main.c -ldl
