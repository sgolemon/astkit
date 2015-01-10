dnl $Id$
dnl config.m4 for extension astkit

PHP_ARG_ENABLE(astkit, whether to enable astkit support,
[  --disable-astkit       Disable astkit support], yes)

if test "$PHP_ASTVIEW" != "no"; then
  PHP_NEW_EXTENSION(astkit, astkit.c astkit-node.c astkit-list.c astkit-decl.c, $ext_shared)
fi
