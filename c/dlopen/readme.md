dlopen
======

This simple program shows how to load a library dynamically and get handle to function so that we can call it and get required output.

#### How to create the .so file

1. First create file with all the function defined in it.
2. Then compile it and create object file. Do not forget to pass ```-fPIC``` flag which stands for ```Position Independent Code```. If we do not add then program will not work unless the library is loaded at specific address.
3. Then create a shared library using the object file

```
gcc -c -fPIC mathHelper.c -o mathHelper.o
gcc -shared -o libmathHelper.so mathHelper.o
```
#### Load The Library
* ```dlopen``` is used to load the required library.
* ```dlsym``` is used to get address of required function from the given library.
* ```dlclose``` is used to unload the library.

```
void *handel = dlopen("./libmathHelper.so", RTLD_LAZY);

[...]

square = (squareProc)dlsym(handel, "square");

[...]

printf("Square of %d is %d\n", number, square(number));

dlclose(handel);

[...]
```

#### Compile The Program
* Compile our program that is using the library.
* The ```-ldl``` specifies the the linker to link the ```libdl.so``` library so that we can use the  ```dlopen```, ```dlsym``` and ```dlclose``` functions.

```
gcc -o main main.c -ldl
```

#### Output
* The program is simple, just print the square of 35.
```
Square of 35 is 1225
```
