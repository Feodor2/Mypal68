function f(a) {
    var k = a;
    T: for (;;) {
        for (;;) {
            for (;;) {
                if (k)
                    continue;
                break T;
            }
        }
    }
}
f(0);
