function f([a, b, c, d]) {
  a = b;
  return function () { return a + b + c + d; };
}

var F = f(["a", "b", "c", "d"]);
assertEq(F(), "bbcd");
