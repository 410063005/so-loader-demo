# 一个用于验证 so 文件符号冲突的小例子

## dlopen

将 `liba.c` 和 `libb.c` 分别编译成共享库，`main.c` 编译成可执行程序 `a.out`，

```
gcc -fPIC --shared liba.c -o liba.so
gcc -fPIC --shared libb.c -o libb.so
```

将 `libb.so` 拷贝一份，重命名为 `liba.so`，

```
cp libb.so libs/liba.so
```

观察 `a.out` 的执行结果。

```
➜  load-so-demo git:(master) ./a.out
hello, world
I will load so from liba.so
hello liba. 版本1
hello liba. 版本1
➜  load-so-demo git:(master) ./a.out libs/liba.so
hello, world
I will load so from libs/liba.so
hello libb. 版本2
hello liba. 版本1
➜  load-so-demo git:(master)
```

可以得到这样的结论：

+ 同一个 `liba.so` 可以被多次加载和链接。`man dlopen` 中提到了这一点，"A second call to dlopen() with the same path will return the same handle"
+ `liba.so` 和 `libs/liba.so` 可以被同时加载和链接，且两个共享库中的 `foo()` 方法都可正常被调用，并不冲突

## JNI

首先编译 `Main.java` 得到类文件，

```
javac Main.java
```

由类文件生成头文件 `Foo.h`，

```
javah Foo
```

将 `liba.c` 和 `libb.c` 分别编译成共享库，

```
gcc -fPIC --shared liba.c -o liba.so
gcc -fPIC --shared libb.c -o libb.so
```

注意 `liba.c` 和 `libb.c` 用到了 JNI 库，所以要记得添加编译如下选项

```
-I/Library/Java/JavaVirtualMachines/jdk1.8.0_191.jdk/Contents/Home/include -I/Library/Java/JavaVirtualMachines/jdk1.8.0_191.jdk/Contents/Home/include/darwin
```

调整 `Main.java` 并观察其运行结果。
