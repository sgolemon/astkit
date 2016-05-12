<?php

function astkit_dump(AstKit $ast, $indent = 0) {
  echo str_repeat(' ', 2 * $indent);
  echo AstKit::kindName($ast->getId());
  if ($attrs = $ast->getAttributes()) {
    printf("-attrs(%04x)", $attrs);
  }
  if ($ast instanceof AstKindDecl) {
    if ($ast->getId() != AstKind::ZEND_AST_CLOSURE) {
      echo ' ', $ast->getName();
      if ($ast->getId() != AstKind::ZEND_AST_CLASS) {
        echo '()';
      }
    }
    if ($flags = $ast->getFlags()) {
      printf(" flags(%04x)", $flags);
    }
  }
  echo "\n";

  $vardump = function($val, $indent = 0) {
    echo str_repeat(' ', 2 * $indent);
    echo "ZEND_AST_ZVAL ";
    if (is_null($val) || is_bool($val) ||
        is_int($val)  ||  is_float($val)) {
      var_dump($val);
    } elseif (is_string($val)) {
      echo 'string(', strlen($val), ') "', urlencode($val), "\"\n";
    } else {
      // shouldn't happen
      echo gettype($val), "\n";
    }
  };

  $children = $ast->numChildren();
  for ($i = 0; $i < $children; ++$i) {
    $child = $ast->getChild($i);
    if ($child === null) continue;
    if ($child instanceof AstKit) {
      astkit_dump($child, $indent + 1);
    } else {
      $vardump($child, $indent + 1);
    }
  }
}
