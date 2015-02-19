#include "astkit.h"

zend_class_entry* astkit_decl_ce = NULL;

/* {{{ proto int AstKitDecl::numChildren() */
static PHP_METHOD(AstKitDecl, numChildren) {
	RETURN_LONG(3);
} /* }}} */

/* {{{ proto object AstKitDecl::getChild(int child) */
ZEND_BEGIN_ARG_INFO(AstKitDecl_getChild_arginfo, 0)
	ZEND_ARG_INFO(0, child)
ZEND_END_ARG_INFO()
static PHP_METHOD(AstKitDecl, getChild) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	zend_long child;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &child) == FAILURE) {
		return;
	}

	if ((child < 0) || (child >= 3)) {
		php_error_docref(NULL, E_WARNING,
		  "Invalid child " ZEND_LONG_FMT ", current node has %d children",
		  child, 3);
		return;
	}
	if (declNode->child[child]) {
		astkit_create_object(return_value, declNode->child[child], objval->tree);
	}
} /* }}} */

/* {{{ proto int AstKitDecl::getLineEnd() */
static PHP_METHOD(AstKitDecl, getLineEnd) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;
	RETURN_LONG(declNode->end_lineno);
} /* }}} */

/* {{{ proto int AstKitDecl::geFlags() */
static PHP_METHOD(AstKitDecl, getFlags) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;
	RETURN_LONG(declNode->flags);
} /* }}} */

/* {{{ proto string AstKitDecl::getDocComment() */
static PHP_METHOD(AstKitDecl, getDocComment) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;
	if (declNode->doc_comment) {
		RETURN_STRINGL(declNode->doc_comment->val, declNode->doc_comment->len);
	} else {
		RETURN_EMPTY_STRING();
	}
} /* }}} */

/* {{{ proto string AstKitDecl::getName() */
static PHP_METHOD(AstKitDecl, getName) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;
	RETURN_STRINGL(declNode->name->val, declNode->name->len);
} /* }}} */

/* {{{ proto bool AstKitDecl::hasParams() */
static PHP_METHOD(AstKitDecl, hasParams) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;
	RETURN_BOOL(declNode->child[0]);
} /* }}} */

/* {{{ proto mixed AstKitDecl::getParams() */
static PHP_METHOD(AstKitDecl, getParams) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;
	astkit_create_object(return_value, declNode->child[0], objval->tree);
} /* }}} */

/* {{{ proto bool AstKitDecl::hasUse() */
static PHP_METHOD(AstKitDecl, hasUse) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;
	RETURN_BOOL(declNode->child[1]);
} /* }}} */

/* {{{ proto mixed AstKitDecl::getUse() */
static PHP_METHOD(AstKitDecl, getUse) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;
	astkit_create_object(return_value, declNode->child[1], objval->tree);
} /* }}} */

/* {{{ proto bool AstKitDecl::hasStatements() */
static PHP_METHOD(AstKitDecl, hasStatements) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;
	RETURN_BOOL(declNode->child[2]);
} /* }}} */

/* {{{ proto mixed AstKitDecl::getStatements() */
static PHP_METHOD(AstKitDecl, getStatements) {
	astkit_object* objval;
	zend_ast_decl* declNode;
	objval = ASTKIT_FETCH_OBJ(getThis());
	declNode = (zend_ast_decl*)objval->node;
	astkit_create_object(return_value, declNode->child[2], objval->tree);
} /* }}} */

static zend_function_entry astkit_decl_methods[] = {
	/* For compat with AstKit, but the hasFoo()/getFoo() APIs are more appropriate */
	PHP_ME(AstKitDecl, numChildren, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitDecl, getChild, AstKitDecl_getChild_arginfo, ZEND_ACC_PUBLIC)

	PHP_ME(AstKitDecl, getLineEnd, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitDecl, getFlags, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitDecl, getDocComment, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitDecl, getName, NULL, ZEND_ACC_PUBLIC)

	PHP_ME(AstKitDecl, hasParams, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitDecl, getParams, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitDecl, hasUse, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitDecl, getUse, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitDecl, hasStatements, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitDecl, getStatements, NULL, ZEND_ACC_PUBLIC)
	/* getLineEnd() */
	PHP_FE_END
};

int astkit_decl_minit(INIT_FUNC_ARGS) {
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "AstKitDecl", astkit_decl_methods);
	astkit_decl_ce = zend_register_internal_class_ex(&ce, astkit_node_ce);

	return SUCCESS;
}
