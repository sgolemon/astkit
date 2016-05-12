--TEST--
AST Export ZEND_AST_ARG_LIST
--FILE--
<?php

echo AstKit::parseString(<<<'EOD'

[];
[null];
[true];
[false];
[42];
[3.14];
[M_PI];
["Hello World"];
[[1,2,3]];
[$x.$y];

[null, true, false, 42, 3.14, M_PI, "Hello World", [1,2,3], $x.$y];
[
  'null' => null,
  'true' => true,
  'false' => false,
  'int' => 42,
  'float' => 3.14,
  /* next index */ M_PI,
  'string' => "Hello World",
  'array' => [1,2,3],
  'expr' => $x . $y,
];

EOD
)->export();

--EXPECT--
[];
[null];
[true];
[false];
[42];
[3.14];
[M_PI];
['Hello World'];
[[1, 2, 3]];
[$x . $y];
[null, true, false, 42, 3.14, M_PI, 'Hello World', [1, 2, 3], $x . $y];
['null' => null, 'true' => true, 'false' => false, 'int' => 42, 'float' => 3.14, M_PI, 'string' => 'Hello World', 'array' => [1, 2, 3], 'expr' => $x . $y];
