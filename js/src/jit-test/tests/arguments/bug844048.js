
function foo() {
  eval("\
    for (var arguments in arguments)\
      assertEq(f(i, 1), i+1);\
  ");
}
foo();

function bar() {
  eval("\
    var arguments;\
    for each(e in [arguments, arguments]) {}\
  ");
}
bar();

(function(){assertEq(typeof eval("var arguments; arguments"), "object")})();
try {
  (function(... rest){assertEq(typeof eval("var arguments; arguments"), "object")})();
  assertEq(false, true);
} catch (e) {
  assertEq(/SyntaxError/.test(e), true);
}
