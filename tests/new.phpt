--TEST--
Create new AST tree with an empty statement list
--FILE--
<?php
include(__DIR__ . '/../util/astkit-dump.php');

$ast = new AstKitList;

astkit_dump($ast);
--EXPECT--
ZEND_AST_STMT_LIST
