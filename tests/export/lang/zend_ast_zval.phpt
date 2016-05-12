--TEST--
AST Export ZEND_AST_ZVAL
--FILE--
<?php

echo AstKit::parseString(<<<'EOD'

null;
true;
false;
123;
234.5;
"foo";
array(1,2,3);
array($x);
EOD
)->export();

--EXPECT--
null;
true;
false;
123;
234.5;
'foo';
[1, 2, 3];
[$x];
