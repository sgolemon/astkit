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

#define ASTKIT_FETCH_OBJ(obj) ((astkit_object*)Z_OBJ_P(obj))

void astkit_create_object(zval* obj, zend_ast* node, astkit_tree* tree);

extern zend_class_entry* astkit_node_ce;
int astkit_node_minit(INIT_FUNC_ARGS);

extern zend_class_entry* astkit_list_ce;
int astkit_list_minit(INIT_FUNC_ARGS);

extern zend_class_entry* astkit_decl_ce;
int astkit_decl_minit(INIT_FUNC_ARGS);

#endif
