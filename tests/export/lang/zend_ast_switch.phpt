--TEST--
AST Export ZEND_AST_SWITCH
--FILE--
<?php

echo AstKit::parseString(<<<'EOD'

switch (foo()) {
  case 1:
    echo "Hello";
    break;
  case 'two':
    echo "Goodbye";
    break;
  case bar():
    exit(1);
  default:
    die(2);
}

EOD
)->export();

--EXPECT--
switch (foo()) {
    case 1:
        echo 'Hello';
        break;
    case 'two':
        echo 'Goodbye';
        break;
    case bar():
        exit(1);
    default:
        exit(2);
}