function F() {
    var T = { };
    try {
        throw 12;
    } catch (e) {
        T.x = 5;
        return T;
    }
}

assertEq((new F()).x, 5);

