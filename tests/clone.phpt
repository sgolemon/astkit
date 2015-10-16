--TEST--
Cloned AST trees
--FILE--
<?php
include(__DIR__ . '/../util/astkit-dump.php');

$ast = AstKit::parseString(<<<EOD
echo "Hello World\n";
function foo() {
  echo "Goodbye\n";
}
EOD
);

$echo = clone $ast->getChild(0)->getChild(0);
$foo = clone $ast->getChild(1);

astkit_dump($ast);
unset($ast);
astkit_dump($echo);
astkit_dump($foo);
--EXPECT--
ZEND_AST_STMT_LIST
  ZEND_AST_STMT_LIST
    ZEND_AST_ECHO
      ZEND_AST_ZVAL string(12) "Hello+World%0A"
  ZEND_AST_FUNC_DECL
    ZEND_AST_PARAM_LIST
    ZEND_AST_STMT_LIST
      ZEND_AST_STMT_LIST
        ZEND_AST_ECHO
          ZEND_AST_ZVAL string(8) "Goodbye%0A"
ZEND_AST_ECHO
  ZEND_AST_ZVAL string(12) "Hello+World%0A"
ZEND_AST_FUNC_DECL
  ZEND_AST_PARAM_LIST
  ZEND_AST_STMT_LIST
    ZEND_AST_STMT_LIST
      ZEND_AST_ECHO
        ZEND_AST_ZVAL string(8) "Goodbye%0A"
