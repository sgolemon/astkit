--TEST--
AST Export ZEND_AST_ENCAPS_LIST
--FILE--
<?php

echo AstKit::parseString(<<<'EOD'

"foo $bar baz";
"foo {$bar} baz";
"foo ${bar} baz";
"foo ${ bar} baz";
"foo ${bar } baz";
"foo ${ bar } baz";
"foo {${bar}} baz";

EOD
)->export();

--EXPECT--
"foo $bar baz";
"foo $bar baz";
"foo $bar baz";
"foo {${bar}} baz";
"foo {${bar}} baz";
"foo {${bar}} baz";
"foo {${bar}} baz";
