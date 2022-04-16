function f(x) {
    if (x) {
        let y;
        y = 12;
        (function () {
          assertEq(y, 12);
        })();
    }
}
f(1);

