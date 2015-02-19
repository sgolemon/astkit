#include "astkit.h"
#include "zend_language_scanner.h"
#include "zend_language_parser.h"

#define ASTKIT_NO_PROCESS 1

zend_class_entry* astkit_node_ce = NULL;
static zend_object_handlers astkit_object_handlers;

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
	ZVAL_NEW_STR(&source, source_string);
	if (zend_prepare_string_for_scanning(&source, "AstKit::parseString()") == FAILURE) {
		zend_restore_lexical_state(&original_lex_state);
		RETURN_FALSE;
	}

	CG(ast) = NULL;
	CG(ast_arena) = zend_arena_create(1024 * 32);
	parse_result = zendparse();
	zval_dtor(&source);
	zend_restore_lexical_state(&original_lex_state);
	if (parse_result != 0) {
		/* parse error */
		zend_ast_destroy(CG(ast));
		zend_arena_destroy(CG(ast_arena));
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
	astkit_create_object(return_value, tree->root, tree);
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
	zend_restore_lexical_state(&original_lex_state);
	zend_destroy_file_handle(&file_handle);
	if (parse_result != 0) {
		/* parse error */
		zend_ast_destroy(CG(ast));
		zend_arena_destroy(CG(ast_arena));
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
	astkit_create_object(return_value, tree->root, tree);
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

/* {{{ proto mixed AstKit::getChild(int $child) */
ZEND_BEGIN_ARG_INFO(AstKit_getChild_arginfo, 0)
	ZEND_ARG_INFO(0, child)
ZEND_END_ARG_INFO()
static PHP_METHOD(AstKit, getChild) {
	astkit_object* objval;
	int numChildren;
	zend_long child;
	objval = ASTKIT_FETCH_OBJ(getThis());
	numChildren = objval->node->kind >> ZEND_AST_NUM_CHILDREN_SHIFT;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &child) == FAILURE) {
		return;
	}

	if ((child < 0) || (child >= numChildren)) {
		php_error_docref(NULL, E_WARNING,
		  "Invalid child " ZEND_LONG_FMT ", current node has %d children",
		  child, numChildren);
		return;
	}

	astkit_create_object(return_value, objval->node->child[child], objval->tree);
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
	PHP_FE_END
};

static zend_object* astkit_node_create(zend_class_entry* ce) {
	astkit_object* object = emalloc(sizeof(astkit_object));
	zend_object_std_init(&(object->std), ce);
	object->std.handlers = &astkit_object_handlers;
	object_properties_init(&(object->std), ce);
	return (zend_object*)object;
}

static void astkit_node_free(zend_object* obj) {
	astkit_object* object = (astkit_object*)obj;
	if (object->tree) {
		if ((--object->tree->refcount) <= 0) {
			zend_ast_destroy(object->tree->root);
			zend_arena_destroy(object->tree->arena);
			efree(object->tree);
		}
	}
	zend_object_std_dtor(obj);
}

int astkit_node_minit(INIT_FUNC_ARGS) {
	zend_class_entry ce;

	memcpy(&astkit_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	astkit_object_handlers.free_obj = astkit_node_free;

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

	return SUCCESS;
}
