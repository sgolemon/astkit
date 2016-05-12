--TEST--
AST Export ZEND_AST_FUNC_DECL
--FILE--
<?php

echo AstKit::parseString(<<<'EOD'

function foo($bar, &$refbar, $baz = "blong", array $qux = null) {
  $refbar = $bar;
  return $baz . ', ' . implode(', ', $qux);
}

EOD
)->export();

--EXPECT--
function foo($bar, &$refbar, $baz = 'blong', array $qux = null) {
    $refbar = $bar;
    return $baz . ', ' . implode(', ', $qux);
}
