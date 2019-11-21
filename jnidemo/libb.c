
#include <stdio.h>
#include "Foo.h"

void foo() {
    printf("%s\n", "hello libb. 版本2");
}

JNIEXPORT void JNICALL Java_Foo_foo
  (JNIEnv *env, jobject obj) {
      foo();
}