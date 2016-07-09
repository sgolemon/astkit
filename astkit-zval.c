#include "astkit.h"

zend_class_entry* astkit_zval_ce = NULL;

/* {{{ proto int AstKitZval::numChildren() */
static PHP_METHOD(AstKitZval, numChildren) {
	RETURN_LONG(0);
} /* }}} */

/* {{{ proto object AstKitZval::getChild(int child[, bool zval_as_value = true]) */
ZEND_BEGIN_ARG_INFO(AstKitZval_getChild_arginfo, 0)
	ZEND_ARG_INFO(0, child)
	ZEND_ARG_INFO(0, zval_as_value)
ZEND_END_ARG_INFO()
static PHP_METHOD(AstKitZval, getChild) {
	RETURN_NULL();
} /* }}} */

/* {{{ proto object AstKitZval::getValue() */
static PHP_METHOD(AstKitZval, getValue) {
	astkit_object* objval = ASTKIT_FETCH_OBJ(getThis());
	zval* val = zend_ast_get_zval(objval->node);
	RETURN_ZVAL(val, 1, 0);
} /* }}} */

static zend_function_entry astkit_zval_methods[] = {
	PHP_ME(AstKitZval, numChildren, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitZval, getChild, AstKitZval_getChild_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(AstKitZval, getValue, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

int astkit_zval_minit(INIT_FUNC_ARGS) {
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "AstKitZval", astkit_zval_methods);
	astkit_zval_ce = zend_register_internal_class_ex(&ce, astkit_node_ce);

	return SUCCESS;
}
