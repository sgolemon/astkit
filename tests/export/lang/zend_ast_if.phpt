--TEST--
AST Export ZEND_AST_IF
--FILE--
<?php

echo AstKit::parseString(<<<'EOD'

if (true) {
  return 1;
} else if ($x)
  return 2;
elseif ($x > $y) {
  return 3;
} else {
  return 4;
}

EOD
)->export();

--EXPECT--
if (true) {
    return 1;
} else if ($x) {
    return 2;
} elseif ($x > $y) {
    return 3;
} else {
    return 4;
}