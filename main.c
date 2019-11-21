
#include <stdio.h>
#include <dlfcn.h>

typedef int (*SIMPLE_FUNC)();

int main(int argc, char *argv[]) {
    //printf("argc = %d\n", argc);

    char *lib_path = "liba.so";
    if (argc >= 2) {
        lib_path = argv[1];
    }

    printf("%s\n", "hello, world");
    printf("I will load so from %s\n", lib_path);

    void* handle = dlopen(lib_path, RTLD_NOW);
    void* sym_foo = dlsym(handle, "foo");
    SIMPLE_FUNC f = sym_foo;
    f();

    void* handle2 = dlopen("liba.so", RTLD_NOW);
    void* sym_foo2 = dlsym(handle2, "foo");
    SIMPLE_FUNC f2 = sym_foo2;
    f2();

    return 0;
}