# 一个用于验证 so 文件符号冲突的小例子

这个小demo用于验证不同 so 文件是否会存在符号重名冲突的问题。符号重名后动态加载器如何处理？

## 参考

Google 关键字 "dlopen duplicate symbol"，找到的一个比较有价值的资料是 [dlopen](https://pubs.opengroup.org/onlinepubs/009695399/functions/dlopen.html)，它提到：

> Symbols introduced into a program through calls to dlopen() may be used in relocation activities. Symbols so introduced may duplicate symbols already defined by the program or previous dlopen() operations. To resolve the ambiguities such a situation might present, the resolution of a symbol reference to symbol definition is based on a symbol resolution order. Two such resolution orders are defined: load or dependency ordering. Load order establishes an ordering among symbol definitions, such that the definition first loaded (including definitions from the image file and any dependent objects loaded with it) has priority over objects added later (via dlopen()).

动态库中出现重名符号时，会基于某种符号解析顺序(symbol resolution order)来确定该如何进行符号解析。有load ordering 和 dependency ordering 两种顺序。**使用 loading ordering 这种顺序时，先加载的符号定义会生效**。


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


JNI 加载 so 代码的运行结果跟直接调用 `dlopen()` 加载 so 不同，如下。

代码一：

```java
class Foo {
   static {
        System.load("/Users/chenming/load-so-demo/jnidemo/liba.so");
    }
    native void foo();
}

public class Main {
    public static void main(String[] args) {
        System.load("/Users/chenming/load-so-demo/jnidemo/libb.so");

        Foo foo = new Foo();
        // 输出 hello liba. 版本2
        foo.foo();
    }
}
```

代码二：

```java
class Foo {
    static {
        System.load("/Users/chenming/load-so-demo/jnidemo/liba.so");
        System.load("/Users/chenming/load-so-demo/jnidemo/libb.so");
    }
    native void foo();
}

public class Main {
    public static void main(String[] args) {
        Foo foo = new Foo();
        // 输出 `hello liba. 版本1`
        foo.foo();
    }
}
```
 
代码三：

```java
class Foo {
    static {
        System.load("/Users/chenming/load-so-demo/jnidemo/liba.so");
    }
    native void foo();
}

public class Main {
    public static void main(String[] args) {

        Foo foo = new Foo();
        // 输出 `hello liba. 版本1`
        foo.foo();
        System.load("/Users/chenming/load-so-demo/jnidemo/libb.so");
        // 输出 `hello liba. 版本1`
        foo.foo();
    }
}
```

从 JNI 代码运行结果来看，如果 `liba.so` 和 `libb.so` 中有同名符号 `foo`，则先被加载的那个 so 中的 `foo()` 函数生效。


参考 《程序员的自我修养》P93
