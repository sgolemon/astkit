--TEST--
Execute a fragment of an AST
--FILE--
<?php
include(__DIR__ . '/../util/astkit-dump.php');

$ast = AstKit::parseString(<<<EOD
echo "Hello World\n";
function foo() {
  return "bar";
}
EOD
);

astkit_dump($ast);
var_dump(function_exists('foo'));
$ast->getChild(0)->execute();
var_dump(function_exists('foo'));
$ast->getChild(1)->execute();
var_dump(function_exists('foo'));
var_dump(foo());

--EXPECT--
ZEND_AST_STMT_LIST
  ZEND_AST_STMT_LIST
    ZEND_AST_ECHO
      ZEND_AST_ZVAL string(12) "Hello+World%0A"
  ZEND_AST_FUNC_DECL
    ZEND_AST_PARAM_LIST
    ZEND_AST_STMT_LIST
      ZEND_AST_RETURN
        ZEND_AST_ZVAL string(3) "bar"
bool(false)
Hello World
bool(false)
bool(true)
string(3) "bar"
