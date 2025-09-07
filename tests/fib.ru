fn fib(n) {
    set a := 0;
    set b := 1;
    for(set i := 0; i < n; i++) {
        brint(a);
        set tmp := a;
        a := b;
        b := tmp + b;
    }
}
fib(10);
