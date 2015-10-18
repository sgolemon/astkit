--TEST--
Graft new value onto an ECHO node
--FILE--
<?php
include(__DIR__ . '/../util/astkit-dump.php');

$ast = AstKit::parseString(<<<EOD
echo "Hello World\n";
EOD
);

astkit_dump($ast);
$ast->getChild(0)->getChild(0)->graft(0, "Goodbyte");
astkit_dump($ast);

--EXPECT--
ZEND_AST_STMT_LIST
  ZEND_AST_STMT_LIST
    ZEND_AST_ECHO
      ZEND_AST_ZVAL string(12) "Hello+World%0A"
ZEND_AST_STMT_LIST
  ZEND_AST_STMT_LIST
    ZEND_AST_ECHO
      ZEND_AST_ZVAL string(8) "Goodbyte"
