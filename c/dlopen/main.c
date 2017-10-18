#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

typedef int (*squareProc)(int number);
squareProc square = NULL;

int main(int argc, char* argv[])
{
    int number = 35;
    void *handel = dlopen("./libmathHelper.so", RTLD_LAZY);

    if(!handel)
    {
        printf("Error: Cannot open libmathHelper.so\n");
        exit(EXIT_FAILURE);
    }

    // Clear the last error if any.
    dlerror();

    // Find the address of 'square' in the library.
    square = (squareProc)dlsym(handel, "square");
    const char *error = dlerror();

    if(error)
    {
        printf("Error: Cannot get symbol 'square': %s\n", error);
        dlclose(handel);
        exit(EXIT_FAILURE);
    }

    printf("Square of %d is %d\n", number, square(number));

    dlclose(handel);
    return 0;
}
