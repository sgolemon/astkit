--TEST--
AST Export ZEND_AST_CLOSURE
--FILE--
<?php

echo AstKit::parseString(<<<'EOD'

function ($a, &$b, $c = "d", stdClass $e = null) {
  $b = $a;
  return $c.((string)$e);
};

EOD
)->export();

--EXPECT--
function ($a, &$b, $c = 'd', stdClass $e = null) {
    $b = $a;
    return $c . (string)$e;
};