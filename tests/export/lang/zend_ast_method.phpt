--TEST--
AST Export ZEND_AST_METHOD
--FILE--
<?php

echo AstKit::parseString(<<<'EOD'

class Foo {
  public function bar($a, &$b, array $c, SPLDoohickie $d = null) {
    return 42;
  }

  protected final static function bar() { return true; }
  private function qux() { return false; }
  function elephpant($x) { return $x; }
}

EOD
)->export();

--EXPECT--
class Foo {
    public function bar($a, &$b, array $c, SPLDoohickie $d = null) {
        return 42;
    }

    protected static final function bar() {
        return true;
    }

    private function qux() {
        return false;
    }

    public function elephpant($x) {
        return $x;
    }

}
