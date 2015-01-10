--TEST--
Basic statements in a psuedomain
--FILE--
<?php
include('astkit-test.inc');

$ast = AstKit::parseString(<<<EOD
echo "Hello World\n";
if (true) {
  print "Goodbye, everyone\n";
}
EOD
);

astkit_test_dump($ast);
--EXPECT--
ZEND_AST_STMT_LIST
  ZEND_AST_STMT_LIST
    ZEND_AST_ECHO
      ZEND_AST_ZVAL string(12) "Hello+World%0A"
  ZEND_AST_IF
    ZEND_AST_IF_ELEM
      ZEND_AST_CONST
        ZEND_AST_ZVAL string(4) "true"
      ZEND_AST_STMT_LIST
        ZEND_AST_PRINT
          ZEND_AST_ZVAL string(18) "Goodbye%2C+everyone%0A"
