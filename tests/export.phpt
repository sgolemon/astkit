--TEST--
Export a portion of a parsed AST
--FILE--
<?php

$ast = AstKit::parseString(<<<'EOD'

function foo($bar, $baz = 123) {
  echo $bar + $baz;
}

function qux(array $bling) {
  var_dump($bling);
}

EOD
);

/* Simple function declaration iterator, doesn't catch all PHP cases... */
$children = $ast->numChildren();
for ($i = 0; $i < $children; ++$i) {
  $node = $ast->getChild($i);
  if (($node instanceof AstKitDecl) &&
      ($node->getId() == AstKit::ZEND_AST_FUNC_DECL)) {
    echo "Definition of ", $node->getName(), "(";
    $params = $node->getParams();
    if ($params) { echo $params->export(); }
    echo "):\n";
    echo "----\n";
    $stmts = $node->getStatements();
    if ($stmts) { echo $stmts->export(), "\n"; }
  }
}

--EXPECT--
Definition of foo($bar, $baz = 123):
----
echo $bar + $baz;

Definition of qux(array $bling):
----
var_dump($bling);