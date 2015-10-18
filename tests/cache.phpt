--TEST--
Make sure astkit nodes are cached, not recreated
--FILE--
<?php

$ast = AstKit::parseString(<<<EOD
echo "Hello World\n";
EOD
);

$echo1 = $ast->getChild(0);
$echo2 = $ast->getChild(0);
var_dump($echo1 === $echo2);
--EXPECT--
bool(true)
