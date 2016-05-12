--TEST--
AST Export ZEND_AST_ARG_LIST
--FILE--
<?php

echo AstKit::parseString(<<<'EOD'

function foo(
  $a,
  &$b,
  array $c,
  array $d = null,
  bool $e = true,
  int $f = 42,
  float $g = 3.14,
  string $h = "hello",
  stdClass $i = null) {
  return $e;
}

EOD
)->export();

--EXPECT--
function foo($a, &$b, array $c, array $d = null, bool $e = true, int $f = 42, float $g = 3.14, string $h = 'hello', stdClass $i = null) {
    return $e;
}