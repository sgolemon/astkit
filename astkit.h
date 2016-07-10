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
	zend_ast* node;
	astkit_tree* tree;
	zend_object std;
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

	/* Cache of zend_ast* nodes to object instances */
	HashTable cache;

	/* Nodes which are no longer attached to their root */
	HashTable orphan;
ZEND_END_MODULE_GLOBALS(astkit)

#if defined(ZTS) && defined(COMPILE_DL_ASTKIT)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

ZEND_EXTERN_MODULE_GLOBALS(astkit)
#define ASTKITG(v) ZEND_MODULE_GLOBALS_ACCESSOR(astkit, v)

void astkit_create_object(zval*, zend_ast*, astkit_tree*, zend_bool);

extern zend_class_entry* astkit_node_ce;
int astkit_node_minit(INIT_FUNC_ARGS);
int astkit_node_mshutdown(SHUTDOWN_FUNC_ARGS);
zend_ast* astkit_ast_copy(zend_ast* ast);
void astkit_graft(astkit_object* objval, int child, zval* value);

extern zend_class_entry* astkit_list_ce;
int astkit_list_minit(INIT_FUNC_ARGS);

extern zend_class_entry* astkit_decl_ce;
int astkit_decl_minit(INIT_FUNC_ARGS);

extern zend_class_entry* astkit_zval_ce;
int astkit_zval_minit(INIT_FUNC_ARGS);

static inline astkit_object* astkit_from_zend_object(zend_object *obj) {
	return ((astkit_object*)(obj + 1)) - 1;
}

static inline zend_object* astkit_to_zend_object(astkit_object* obj) {
	return &(obj->std);
}

static inline astkit_object* ASTKIT_FETCH_OBJ(zval *obj) {
	ZEND_ASSERT(Z_TYPE_P(obj) == IS_OBJECT);
	ZEND_ASSERT(instanceof_function(Z_OBJCE_P(obj), astkit_node_ce));
	return astkit_from_zend_object(Z_OBJ_P(obj));
}

#endif
