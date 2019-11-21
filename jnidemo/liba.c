
#include <stdio.h>
#include "Foo.h"

void foo() {
    printf("%s\n", "hello liba. 版本1");
}

JNIEXPORT void JNICALL Java_Foo_foo
  (JNIEnv *env, jobject obj) {
      foo();
}