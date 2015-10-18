--TEST--
Graft new value onto a statement list replacing a non zval node
--FILE--
<?php

$if = AstKit::parseString(<<<EOD
if (true) {
  echo "This is a triumph.\n";
} else {
  echo "The cake is a lie.\n";
}
EOD
);

$if->execute();
$const = $if->getChild(0)->getChild(0);
$const->graft(0, false);
$if->execute();
--EXPECT--
This is a triumph.
The cake is a lie.
