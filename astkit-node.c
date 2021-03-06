#include "astkit.h"
#include "zend_language_scanner.h"
#include "zend_language_parser.h"

#define ASTKIT_NO_PROCESS 1

zend_class_entry* astkit_node_ce = NULL;
static zend_object_handlers astkit_object_handlers;
static void astkit_ast_destroy(zend_ast* ast);

/* {{{ proto string AstKit::kindName(int $kind) */
ZEND_BEGIN_ARG_INFO(AstKind_kindName_arginfo, 0)
	ZEND_ARG_INFO(0, kind)
ZEND_END_ARG_INFO();
static PHP_METHOD(AstKit, kindName) {
	zend_long kind;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &kind) == FAILURE) {
		return;
	}

	switch (kind) {
#define AST(id) case ZEND_AST_##id: RETURN_STRING("ZEND_AST_" #id);
#define AST_DECL(id) AST(id)
#define AST_LIST(id) AST(id)
#define AST_CHILD(id, children) AST(id)
#include "ast-names.h"
#undef AST_CHILD
#undef AST_LIST
#undef AST_DECL
#undef AST
		default:
			php_error_docref(NULL, E_WARNING, "Unknown AST kind: " ZEND_LONG_FMT, kind);
			RETURN_NULL();
	}
} /* }}} */

/* {{{ proto AstKitList AstKit::parseString(string $code) */
ZEND_BEGIN_ARG_INFO(AstKind_parseString_arginfo, 0)
	ZEND_ARG_INFO(0, code)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()
static PHP_METHOD(AstKit, parseString) {
	zend_string* source_string;
	zend_long options = 0;
	zval source;
	zend_lex_state original_lex_state;
	astkit_tree *tree;
	int parse_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|l", &source_string, &options) == FAILURE) {
		return;
	}

	zend_save_lexical_state(&original_lex_state);
	ZVAL_STR_COPY(&source, source_string);
	if (zend_prepare_string_for_scanning(&source, "AstKit::parseString()") == FAILURE) {
		zend_restore_lexical_state(&original_lex_state);
		RETURN_FALSE;
	}

	CG(ast) = NULL;
	CG(ast_arena) = zend_arena_create(1024 * 32);
	parse_result = zendparse();
	zval_dtor(&source);
	if (parse_result != 0) {
		/* parse error */
		zend_ast_destroy(CG(ast));
		zend_arena_destroy(CG(ast_arena));
		CG(ast) = NULL;
		CG(ast_arena) = NULL;
		zend_restore_lexical_state(&original_lex_state);
		RETURN_FALSE;
	}

	if (zend_ast_process &&
	    !(options & ASTKIT_NO_PROCESS)) {
		zend_ast_process(CG(ast));
	}
	tree = emalloc(sizeof(astkit_tree));
	tree->root = CG(ast);
	tree->arena = CG(ast_arena);
	tree->refcount = 0;

	CG(ast) = NULL;
	CG(ast_arena) = NULL;
	zend_restore_lexical_state(&original_lex_state);
	astkit_create_object(return_value, tree->root, tree, 0);
} /* }}} */

/* {{{ proto AstKitList AstKit::parseFile(string $filename) */
ZEND_BEGIN_ARG_INFO(AstKind_parseFile_arginfo, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()
static PHP_METHOD(AstKit, parseFile) {
	char *filename;
	size_t filename_len;
	zend_long options = 0;
	zend_file_handle file_handle;
	zend_lex_state original_lex_state;
	astkit_tree *tree;
	int parse_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|l", &filename, &filename_len, &options) == FAILURE) {
		return;
	}

	if (FAILURE == zend_stream_open(filename, &file_handle)) {
		/* TODO: throw an error? */
		return;
	}
	if (open_file_for_scanning(&file_handle)==FAILURE) {
		zend_destroy_file_handle(&file_handle);
		return;
	}

	CG(ast) = NULL;
	CG(ast_arena) = zend_arena_create(1024 * 32);
	zend_save_lexical_state(&original_lex_state);
	parse_result = zendparse();
	if (parse_result != 0) {
		/* parse error */
		zend_ast_destroy(CG(ast));
		zend_arena_destroy(CG(ast_arena));
		zend_restore_lexical_state(&original_lex_state);
		zend_destroy_file_handle(&file_handle);
		RETURN_FALSE;
	}

	if (zend_ast_process &&
	    !(options & ASTKIT_NO_PROCESS)) {
		zend_ast_process(CG(ast));
	}

	tree = emalloc(sizeof(astkit_tree));
	tree->root = CG(ast);
	tree->arena = CG(ast_arena);
	tree->refcount = 0;

	CG(ast) = NULL;
	CG(ast_arena) = NULL;
	zend_restore_lexical_state(&original_lex_state);
	zend_destroy_file_handle(&file_handle);
	astkit_create_object(return_value, tree->root, tree, 0);
} /* }}} */

/* {{{ proto void AstKit::__construct() */
static PHP_METHOD(AstKit, __construct) {
	/* Never called */
	php_error_docref(NULL, E_ERROR, "AstKit::__construct is private");
} /* }}} */

/* {{{ proto int AstKit::getId() */
static PHP_METHOD(AstKit, getId) {
	astkit_object* objval = ASTKIT_FETCH_OBJ(getThis());
	RETURN_LONG(objval->node->kind);
} /* }}} */

/* {{{ proto int AstKit::getAttributes() */
static PHP_METHOD(AstKit, getAttributes) {
	astkit_object* objval = ASTKIT_FETCH_OBJ(getThis());
	RETURN_LONG(objval->node->attr);
} /* }}} */

/* {{{ proto int AstKit::getLine() */
static PHP_METHOD(AstKit, getLine) {
	astkit_object* objval = ASTKIT_FETCH_OBJ(getThis());
	RETURN_LONG(objval->node->lineno);
} /* }}} */

/* {{{ proto int AstKit::numChildren() */
static PHP_METHOD(AstKit, numChildren) {
	astkit_object* objval = ASTKIT_FETCH_OBJ(getThis());

	RETURN_LONG(objval->node->kind >> ZEND_AST_NUM_CHILDREN_SHIFT);
} /* }}} */

/* {{{ proto mixed AstKit::getChild(int $child[, bool $zval_as_value = true]) */
ZEND_BEGIN_ARG_INFO(AstKit_getChild_arginfo, 0)
	ZEND_ARG_INFO(0, child)
	ZEND_ARG_INFO(0, zval_as_value)
ZEND_END_ARG_INFO()
static PHP_METHOD(AstKit, getChild) {
	astkit_object* objval = ASTKIT_FETCH_OBJ(getThis());
	int numChildren = objval->node->kind >> ZEND_AST_NUM_CHILDREN_SHIFT;
	zend_long child;
	zend_bool zval_as_value = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|b", &child, &zval_as_value) == FAILURE) {
		return;
	}

	if ((child < 0) || (child >= numChildren)) {
		php_error_docref(NULL, E_WARNING,
		  "Invalid child " ZEND_LONG_FMT ", current node has %d children",
		  child, numChildren);
		return;
	}

	astkit_create_object(return_value, objval->node->child[child], objval->tree, zval_as_value);
} /* }}} */

/* {{{ proto string AstKit::export() */
static PHP_METHOD(AstKit, export) {
	astkit_object* objval = ASTKIT_FETCH_OBJ(getThis());
	zend_string *ret = zend_ast_export("", objval->node, "");

	if (ret) {
		RETURN_STR(ret);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ proto void AstKit::graft(int $child, mixed $treeOrValue) */
ZEND_BEGIN_ARG_INFO(AstKit_graft_arginfo, 0)
	ZEND_ARG_INFO(0, child)
	ZEND_ARG_INFO(0, treeOrValue)
ZEND_END_ARG_INFO();
static PHP_METHOD(AstKit, graft) {
	astkit_object* objval = ASTKIT_FETCH_OBJ(getThis());
	zend_long child;
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lz", &child, &value) == FAILURE) {
		return;
	}

	astkit_graft(objval, child, value);
} /* }}} */

/* {{{ astkit_graft */
void astkit_graft(astkit_object* objval, int child, zval* value) {
	zend_arena *old_arena;
	zend_ast *new_child;

	if (child < 0) {
		php_error_docref(NULL, E_WARNING, "Invalid child number, must be positive integer");
		return;
	}

	old_arena = CG(ast_arena);
	CG(ast_arena) = objval->tree->arena;

	if ((Z_TYPE_P(value) == IS_OBJECT) &&
	    instanceof_function(Z_OBJCE_P(value), astkit_node_ce)) {
		new_child = astkit_ast_copy(ASTKIT_FETCH_OBJ(value)->node);
	} else if (Z_TYPE_P(value) != IS_OBJECT) {
		new_child = zend_ast_create_zval(value);
	} else {
		php_error_docref(NULL, E_WARNING, "Unable t graft objects into AST nodes");
		CG(ast_arena) = old_arena;
		return;
	}
	CG(ast_arena) = old_arena;

	if ((objval->node->kind >> ZEND_AST_SPECIAL_SHIFT) == 1) {
		/* Don't allow grafting decls, it gets... weird.  There are better ways, honey. */
		php_error_docref(NULL, E_WARNING, "Unable to graft onto Decl nodes directly.");
		return;
	} else if ((objval->node->kind >> ZEND_AST_IS_LIST_SHIFT) == 1) {
		zend_ast_list *list = zend_ast_get_list(objval->node);
		zend_bool append = child >= list->children;
		if (append) {
			/* Append new child */
			zend_ast_list_add(objval->node, new_child);
		} else {
			/* Replace existing child */
			old_arena = CG(ast_arena);
			CG(ast_arena) = objval->tree->arena;
			astkit_ast_destroy(list->child[child]);
			CG(ast_arena) = old_arena;

			list->child[child] = new_child;
		}
	} else {
		int numChildren = objval->node->kind >> ZEND_AST_NUM_CHILDREN_SHIFT;
		if (child >= numChildren) {
			zend_ast_destroy(new_child);
			php_error_docref(NULL, E_WARNING, "Node only has %d child slots, cannot graft onto child %d",
			                 numChildren, child);
			return;
		}

		old_arena = CG(ast_arena);
		CG(ast_arena) = objval->tree->arena;
		astkit_ast_destroy(objval->node->child[child]);
		CG(ast_arena) = old_arena;

		objval->node->child[child] = new_child;
	}
} /* }}} */

/* {{{ proto mixed AstKit::execute() */
static PHP_METHOD(AstKit, execute) {
	astkit_object* objval = ASTKIT_FETCH_OBJ(getThis());

	zend_arena *old_arena = CG(ast_arena);

	ASTKITG(hijack_ast_arena) = CG(ast_arena) = zend_arena_create(32 * 1024);
	ASTKITG(hijack_ast) = astkit_ast_copy(objval->node);

	CG(ast_arena) = old_arena;

	char *eval_name = zend_make_compiled_string_description("AstKit::execute");
	zend_eval_string("", return_value, eval_name);
	efree(eval_name);
} /* }}} */

static zend_function_entry astkit_node_methods[] = {
	PHP_ME(AstKit, kindName, AstKind_kindName_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(AstKit, parseString, AstKind_parseString_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(AstKit, parseFile, AstKind_parseFile_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	PHP_ME(AstKit, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(AstKit, getId, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKit, getAttributes, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKit, getLine, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKit, numChildren, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKit, getChild, AstKit_getChild_arginfo, ZEND_ACC_PUBLIC)

	PHP_ME(AstKit, export, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKit, graft, AstKit_graft_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(AstKit, execute, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/* Can't use zend_ast_copy() because it uses emalloc instead of zend_arena_alloc
 * and it doesn't handle copying declarations.
 */
static inline size_t zend_ast_size(uint32_t children) {
        return sizeof(zend_ast) - sizeof(zend_ast *) + sizeof(zend_ast *) * children;
}
static inline size_t zend_ast_list_size(uint32_t children) {
        return sizeof(zend_ast_list) - sizeof(zend_ast *) + sizeof(zend_ast *) * children;
}
zend_ast* astkit_ast_copy(zend_ast* ast) {
	if (ast == NULL) {
		return NULL;
	} else if (ast->kind == ZEND_AST_ZVAL) {
		zend_ast_zval *new = zend_arena_alloc(&CG(ast_arena), sizeof(zend_ast_zval));
		*new = *((zend_ast_zval*)ast);
		zval_copy_ctor(&new->val);
		return (zend_ast*)new;
	} else if (ast->kind == ZEND_AST_ZNODE) {
		php_error_docref(NULL, E_WARNING, "Encountered unexpected AST_ZNODE");
		return NULL;
	} else if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		zend_ast_list *new = zend_arena_alloc(&CG(ast_arena), zend_ast_list_size(list->children));
		uint32_t i;
		*new = *list;
		for (i = 0; i < new->children; ++i) {
			new->child[i] = astkit_ast_copy(list->child[i]);
		}
		return (zend_ast*)new;
	} else if (ast->kind < (1 << ZEND_AST_IS_LIST_SHIFT)) {
		uint32_t i;
		zend_ast_decl *decl = (zend_ast_decl*)ast;
		zend_ast_decl *new = zend_arena_alloc(&CG(ast_arena), sizeof(zend_ast_decl));
		*new = *decl;
		if (new->doc_comment) zend_string_addref(new->doc_comment);
		if (new->name) zend_string_addref(new->name);
		for (i = 0; i < 4; i++) {
			new->child[i] = astkit_ast_copy(decl->child[i]);
		}
		return (zend_ast*)new;
	} else {
		uint32_t i, children = zend_ast_get_num_children(ast);
		zend_ast *new = zend_arena_alloc(&CG(ast_arena), zend_ast_size(children));
		*new = *ast;
		for (i = 0; i < children; i++) {
			new->child[i] = astkit_ast_copy(ast->child[i]);
		}
		return new;
	}
}

static void astkit_ast_destroy(zend_ast* ast) {
	zval *obj;
	if (!ast) return;
	obj = zend_hash_index_find(&ASTKITG(cache), (zend_ulong)ast);
	if (obj) {
		/* Owned by an object somewhere, mark it as orphaned from the root */
		zval undef;
		ZVAL_UNDEF(&undef);
		zend_hash_index_add(&ASTKITG(orphan), (zend_ulong)ast, &undef);
		return;
	}

	if (ast->kind == ZEND_AST_ZVAL) {
		/* No children to scan through */
	} else if (ast->kind == ZEND_AST_ZNODE) {
		php_error_docref(NULL, E_WARNING, "Encountered unexpected AST_ZNODE");
	} else if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		uint32_t i;
		for (i = 0; i < list->children; ++i) {
			astkit_ast_destroy(list->child[i]);
		}
		list->children = 0;
	} else if (ast->kind < (1 << ZEND_AST_IS_LIST_SHIFT)) {
		uint32_t i;
		zend_ast_decl *decl = (zend_ast_decl*)ast;
		for (i = 0; i < 4; i++) {
			astkit_ast_destroy(decl->child[i]);
			decl->child[i] = NULL;
		}
	} else {
		uint32_t i, children = zend_ast_get_num_children(ast);
		for (i = 0; i < children; i++) {
			astkit_ast_destroy(ast->child[i]);
			ast->child[i] = NULL;
		}
	}
	zend_ast_destroy(ast);
}

static zend_object* astkit_node_create(zend_class_entry* ce) {
	astkit_object* object = ecalloc(1, sizeof(astkit_object) + zend_object_properties_size(ce));
	zend_object* ret = astkit_to_zend_object(object);
	zend_object_std_init(ret, ce);
	ret->handlers = &astkit_object_handlers;
	object_properties_init(ret, ce);
	return ret;
}

static zend_object* astkit_node_clone(zval *srcObj) {
	zend_arena *old_arena = CG(ast_arena);
	astkit_object* src = ASTKIT_FETCH_OBJ(srcObj);
	astkit_object* dest = astkit_from_zend_object(astkit_node_create(Z_OBJCE_P(srcObj)));
	zend_object* ret = astkit_to_zend_object(dest);
	zend_objects_clone_members(ret, astkit_to_zend_object(src));

	dest->tree = emalloc(sizeof(astkit_tree));
	dest->tree->arena = CG(ast_arena) = zend_arena_create(1024 * 32);
	dest->tree->refcount = 1;
	dest->tree->root = dest->node = astkit_ast_copy(src->node);

	CG(ast_arena) = old_arena;
	return ret;
}

static void astkit_node_free(zend_object* obj) {
	astkit_object* object = astkit_from_zend_object(obj);
	zend_hash_index_del(&ASTKITG(cache), (zend_ulong)object->node);
	if (zend_hash_index_exists(&ASTKITG(orphan), (zend_ulong)object->node)) {
		astkit_ast_destroy(object->node);
		zend_hash_index_del(&ASTKITG(orphan), (zend_ulong)object->node);
	}
	if (object->tree) {
		if ((--object->tree->refcount) <= 0) {
			zend_ast_destroy(object->tree->root);
			zend_arena_destroy(object->tree->arena);
			efree(object->tree);
		}
	}
	zend_object_std_dtor(obj);
}

static zend_op_array *(*old_compile_string)(zval*, char*);
static zend_op_array *astkit_compile_string(zval* source_string, char* filename) {
	if (!ASTKITG(hijack_ast)) {
		return old_compile_string(source_string, filename);
	}

	/* Hijack in progress, bypass any opcache */
	return compile_string(source_string, filename);
}

static void (*old_ast_process)(zend_ast*);
static void astkit_ast_process(zend_ast* ast) {
	ZEND_ASSERT(ast == CG(ast));
	if (ASTKITG(hijack_ast)) {
		/* Destroy the dummy ast, swap in ours, and release it */
		zend_ast_destroy(CG(ast));
		zend_arena_destroy(CG(ast_arena));

		CG(ast) = ASTKITG(hijack_ast);
		CG(ast_arena) = ASTKITG(hijack_ast_arena);

		ASTKITG(hijack_ast) = NULL;
		ASTKITG(hijack_ast_arena) = NULL;
	}
	if (old_ast_process) {
		old_ast_process(CG(ast));
	}
}

int astkit_node_minit(INIT_FUNC_ARGS) {
	zend_class_entry ce;

	memcpy(&astkit_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    astkit_object_handlers.offset = XtOffsetOf(astkit_object, std);
	astkit_object_handlers.free_obj = astkit_node_free;
	astkit_object_handlers.clone_obj = astkit_node_clone;

	INIT_CLASS_ENTRY(ce, "AstKit", astkit_node_methods);
	astkit_node_ce = zend_register_internal_class(&ce);
	astkit_node_ce->create_object = astkit_node_create;

	zend_declare_class_constant_long(astkit_node_ce, "NO_PROCESS", sizeof("NO_PROCESS") - 1, ASTKIT_NO_PROCESS);

#define AST(id) zend_declare_class_constant_long(astkit_node_ce, "ZEND_AST_" #id, sizeof("ZEND_AST_" #id) - 1, ZEND_AST_##id);
#define AST_DECL(id) AST(id)
#define AST_LIST(id) AST(id)
#define AST_CHILD(id, children) AST(id)
#include "ast-names.h"
#undef AST_CHILD
#undef AST_LIST
#undef AST_DECL
#undef AST

#define AST_CONST(id) zend_declare_class_constant_long(astkit_node_ce, #id, sizeof(#id) - 1, id);
#include "constants.h"
#undef AST_CONST

	old_compile_string = zend_compile_string;
	zend_compile_string = astkit_compile_string;

	old_ast_process = zend_ast_process;
	zend_ast_process = astkit_ast_process;

	return SUCCESS;
}

int astkit_node_mshutdown(SHUTDOWN_FUNC_ARGS) {
	zend_compile_string = old_compile_string;
	zend_ast_process = old_ast_process;
	return SUCCESS;
}
