/*
  +----------------------------------------------------------------------+
  | PHP Version 7							|
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group				|
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is	|
  | available through the world-wide-web at the following url:	   |
  | http://www.php.net/license/3_01.txt				  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to	  |
  | license@php.net so we can mail you a copy immediately.	       |
  +----------------------------------------------------------------------+
  | Author: Sara Golemon <pollita@php.net>			       |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "astkit.h"

#include "ext/standard/info.h"

#if PHP_MAJOR_VERSION < 7
# error AstKit requires PHP version 7 or later
#endif

/* ZNODE is a special AST node which only appears in the compile stage */
#define astkit_create_object_ZNODE astkit_create_object_unexpected

static void astkit_create_object_ZVAL(zval* obj, zend_ast* node) {
	zval* val = zend_ast_get_zval(node);
	ZVAL_ZVAL(obj, val, 1, 0);
}

static void astkit_create_object_unexpected(zval* obj, zend_ast* node) {
	php_error_docref(NULL, E_WARNING, "Unknown AST node type: %d", (int)node->kind);
	ZVAL_FALSE(obj);
}

void astkit_create_object(zval* obj, zend_ast* node, astkit_tree* tree) {
	zval *cached_object;
	zend_class_entry *ce = NULL;

	if (!node) {
		ZVAL_NULL(obj);
		return;
	}

	cached_object = zend_hash_index_find(&ASTKITG(cache), (zend_ulong)node);
	if (cached_object) {
		ZVAL_COPY(obj, cached_object);
		return;
	}

	switch (node->kind) {
#define AST(id) case ZEND_AST_##id: astkit_create_object_##id(obj, node); return;
#define AST_DECL(id) case ZEND_AST_##id: ce = astkit_decl_ce; break;
#define AST_LIST(id) case ZEND_AST_##id: ce = astkit_list_ce; break;
#define AST_CHILD(id, children) case ZEND_AST_##id: ce = astkit_node_ce; break;
#include "ast-names.h"
#undef AST_CHILD
#undef AST_LIST
#undef AST_DECL
#undef AST
		default:
			astkit_create_object_unexpected(obj, node);
	}

	astkit_object* objval;
	object_init_ex(obj, ce);
	objval = ASTKIT_FETCH_OBJ(obj);
	objval->node = node;
	objval->tree = tree;
	++tree->refcount;

	zend_hash_index_add(&ASTKITG(cache), (zend_ulong)node, obj);
}

/* {{{ PHP_MINI_FUNCTION */
PHP_MINIT_FUNCTION(astkit) {
	return ((astkit_node_minit(INIT_FUNC_ARGS_PASSTHRU) == SUCCESS) &&
		(astkit_list_minit(INIT_FUNC_ARGS_PASSTHRU) == SUCCESS) &&
		(astkit_decl_minit(INIT_FUNC_ARGS_PASSTHRU) == SUCCESS))
	       ? SUCCESS : FAILURE;
} /* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(astkit) {
	return ((astkit_node_mshutdown(SHUTDOWN_FUNC_ARGS_PASSTHRU) == SUCCESS))
	       ? SUCCESS : FAILURE;
} /* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(astkit) {
	php_info_print_table_start();
	php_info_print_table_row(2, "astkit support", "enabled");
	php_info_print_table_end();
} /* }}} */

ZEND_DECLARE_MODULE_GLOBALS(astkit);

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(astkit) {
#if defined(COMPILE_DL_ASTKIT) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	astkit_globals->hijack_ast = NULL;
	astkit_globals->hijack_ast_arena = NULL;
	zend_hash_init(&(astkit_globals->cache), 32, NULL, NULL, 1);
}
/* }}} */

/* {{{ astkit_module_entry
 */
zend_module_entry astkit_module_entry = {
	STANDARD_MODULE_HEADER,
	"astkit",
	NULL, /* functions */
	PHP_MINIT(astkit),
	PHP_MSHUTDOWN(astkit),
	NULL, /* RINIT */
	NULL, /* RSHUTDOWN */
	PHP_MINFO(astkit),
	"7.0.0-dev",
	PHP_MODULE_GLOBALS(astkit),
	PHP_GINIT(astkit),
	NULL, /* GSHUTDOWN */
	NULL, /* RPOSTSHUTDOWN */
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_ASTKIT
ZEND_GET_MODULE(astkit)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
