PHP7 AST exporer

**NOTE: PHP7 NEEDS A VERY SMALL PATCH IN ORDER FOR THIS TO COMPILE**

When compiling php7-master, first edit `Zend/zend_globals_macros.h` and add `ZEND_API` to the line which reads: `int zendparse(void);`.  You'll find this somewhere around line 39 (depending if other patches have landed).  If you prefer a patch file, use this:

```
diff --git a/Zend/zend_globals_macros.h b/Zend/zend_globals_macros.h
index b8fb763..190905f 100644
--- a/Zend/zend_globals_macros.h
+++ b/Zend/zend_globals_macros.h
@@ -36,7 +36,7 @@ BEGIN_EXTERN_C()
 # define CG(v) (compiler_globals.v)
 extern ZEND_API struct _zend_compiler_globals compiler_globals;
 #endif
-int zendparse(void);
+ZEND_API int zendparse(void);
 
 
 /* Executor */
```

### The API

```
class AstKit {
  // All ZEND_AST_* enums are registered as constants
  const int ZEND_AST_* = *;

  /**
   * Option to parseString/parseFile
   *
   * Disable any zend_ast_hook
   * which may have been registered
   * by another extension
   */
  const int NO_PROCESS = 1;

  /**
   * You don't new these objects.
   * See AstKit::parseString() and AstKit::parseFile()
   * as well as various get*() methods
   */
  private function __construct();

  /**
   * Translate a ZEND_AST_* enum to its human readable name
   */
  static public function kindName(int $kind): string;

  /**
   * Generate an AST from a PHP string
   */
  static public function parseString(string $code,
                                     int $options = 0): ?AstKit;

  /**
   * Generate an AST from a PHP file
   */
  static public function parseFile(string $filename,
                                   int $options = 0): ?AstKit;

  /**
   * Get the ZEND_AST_* kind associated with this node
   */
  public function getId(): int;

  /**
   * Get the attributes for this node
   */
  public function getAttributes(): int;

  /**
   * Get the line this AST node appeared on
   */
  public function getLine(): int;

  /**
   * How many children does this node have?
   */
  public function numChildren(): int;

  /**
   * Get one of this node's children [0, numChildren() - 1]
   */
  public function getChild(int $child): ?AstKit;
}

/**
 * Specialization of AstKit for List style nodes
 *
 * The API is the same as AstKit,
 * but there are internal differences which make
 * having a separate class better
 *
 * Note that while numChildren/getChild can be used
 * Declarations have specific child layouts,
 * so the hasX()/getX() may be more useful
 * and less brittle.
 */
class AstKitList extends AstKit {}

/**
 * Specialization of AstKit for declarations
 */
class AstKitDecl extends AstKit {
  /**
   * Line number on which the declaration ends
   */
  public function getLineEnd(): int;

  /**
   * Declaration specific flags
   */
  public function getFlags(): int;

  /**
   * The Doc Comment, duh...
   */
  public function getDocComment(): string;

  /**
   * The name of the thing being declared
   */
  public function getName(): string;

  /**
   * Is there a parameter list for this function/method
   */
  public function hasParams(): bool;

  /**
   * Get the parameter list
   */
  public function getParams(): ?AstKit;

  /**
   * Are there use variable bound to this closure?
   */
  public function hasUse(): bool;

  /**
   * Get the use list
   */
  public function getUse(): ?AstKit;

  /**
   * Are there statements associated with this declaration
   */
  public function hasStatements(): bool;

  /**
   * Get the statements associated with this declaration
   */
  public function getStatements(): ?AstKit;
}
```
