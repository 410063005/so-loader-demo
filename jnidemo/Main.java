
class Foo {
    static {
        System.load("/Users/chenming/load-so-demo/jnidemo/liba.so");
    }
    native void foo();
}

public class Main {
    public static void main(String[] args) {
        System.load("/Users/chenming/load-so-demo/jnidemo/libb.so");

        System.load("/Users/chenming/load-so-demo/jnidemo/liba.so");


        Foo foo = new Foo();
        foo.foo();
    }
}