#ifndef incl_PHP_ASTKIT_H
#define incl_PHP_ASTKIT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend_ast.h"
#include "zend_arena.h"

typedef struct _astkit_tree {
	zend_ast* root;
	zend_arena* arena;
	int refcount;
} astkit_tree;

typedef struct _astkit_object {
	zend_object std;
	zend_ast* node;
	astkit_tree* tree;
} astkit_object;

ZEND_BEGIN_MODULE_GLOBALS(astkit)
	/* When non-null, the next invocation of eval() will:
	 * * Bypass any opcache in use
	 * * Destroy the dummy ast/ast_arena during ast_process
	 * * Replace them with these
	 *
	 * It was that, or reimplement the compiler...
         */
	zend_ast* hijack_ast;
	zend_arena* hijack_ast_arena;
ZEND_END_MODULE_GLOBALS(astkit)

#if defined(ZTS) && defined(COMPILE_DL_ASTKIT)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

ZEND_EXTERN_MODULE_GLOBALS(astkit)
#define ASTKITG(v) ZEND_MODULE_GLOBALS_ACCESSOR(astkit, v)

#define ASTKIT_FETCH_OBJ(obj) ((astkit_object*)Z_OBJ_P(obj))

void astkit_create_object(zval* obj, zend_ast* node, astkit_tree* tree);

extern zend_class_entry* astkit_node_ce;
int astkit_node_minit(INIT_FUNC_ARGS);
int astkit_node_mshutdown(SHUTDOWN_FUNC_ARGS);
zend_ast* astkit_ast_copy(zend_ast* ast);

extern zend_class_entry* astkit_list_ce;
int astkit_list_minit(INIT_FUNC_ARGS);

extern zend_class_entry* astkit_decl_ce;
int astkit_decl_minit(INIT_FUNC_ARGS);

#endif
