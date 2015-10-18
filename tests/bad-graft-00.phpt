--TEST--
Graft statement into non statement list 00
--FILE--
<?php

include(__DIR__ . '/../util/astkit-dump.php');

$ast = new AstKitList;
$ast->graft(0, "foo");

astkit_dump($ast);

$ast->execute();
echo "Done\n";
--EXPECT--
ZEND_AST_STMT_LIST
  ZEND_AST_ZVAL string(3) "foo"
Done
