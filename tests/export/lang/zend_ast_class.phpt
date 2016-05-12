--TEST--
AST Export ZEND_AST_CLASS
--FILE--
<?php

echo AstKit::parseString(<<<'EOD'

class Foo extends Bar implements Baz, Qux {}

EOD
)->export();

--EXPECT--
class Foo extends Bar implements Baz, Qux {
}
