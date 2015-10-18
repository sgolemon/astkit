--TEST--
Test cleaning up for orphaned nodes
--FILE--
<?php
include(__DIR__ . '/../util/astkit-dump.php');

$ast = AstKit::parseString(<<<EOD
echo strtolower("Hello\n");
EOD
);

$echo = $ast->getChild(0)->getChild(0);
$call = $echo->getChild(0);

$echo->graft(0, "no\n");

astkit_dump($ast);
--EXPECT--
ZEND_AST_STMT_LIST
  ZEND_AST_STMT_LIST
    ZEND_AST_ECHO
      ZEND_AST_ZVAL string(3) "no%0A"

