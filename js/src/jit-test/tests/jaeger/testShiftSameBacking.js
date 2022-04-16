function f(a) {
    var x = a;
    var y = x;

    assertEq((x << y), (a << a));
    assertEq((y << x), (a << a));
}

f(2);

