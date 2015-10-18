--TEST--
Graft new value onto a statement list
--FILE--
<?php
include(__DIR__ . '/../util/astkit-dump.php');

$echo = AstKit::parseString(<<<EOD
echo "Hi there!\n";
EOD
);

// Strip off unnecessary STMT_LIST nodes
$echo = $echo->getChild(0)->getChild(0);

$ast = new AstKitList;
$ast->graft(0, $echo);
$ast->graft(1, $echo);
$ast->append($echo);

astkit_dump($ast);
$ast->execute();

--EXPECT--
ZEND_AST_STMT_LIST
  ZEND_AST_ECHO
    ZEND_AST_ZVAL string(10) "Hi+there%21%0A"
  ZEND_AST_ECHO
    ZEND_AST_ZVAL string(10) "Hi+there%21%0A"
  ZEND_AST_ECHO
    ZEND_AST_ZVAL string(10) "Hi+there%21%0A"
Hi there!
Hi there!
Hi there!
