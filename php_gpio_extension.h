/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:  Barotte Simon <simon.barotte@gmail.com>                     |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_GPIO_EXTENSION_H
#define PHP_GPIO_EXTENSION_H

extern zend_module_entry gpio_extension_module_entry;
#define phpext_gpio_extension_ptr &gpio_extension_module_entry

#define PHP_GPIO_EXTENSION_VERSION "0.1" /* Replace with version number for your extension */

/* List of valid pin for raspberry version can be better maybe... */
#define B_REV_1_CNT 17
#define B_REV_1 {3, 5, 7, 11, 13, 15, 19, 21, 23, 8, 10, 12, 16, 18, 22, 24, 26}

#define B_REV_2_CNT 17
#define B_REV_2 {3, 5, 7, 11, 13, 15, 19, 21, 23, 8, 10, 12, 16, 18, 22, 24, 26}

#define B_PLUS_CNT 26
#define B_PLUS {2, 3, 4, 17, 27, 22, 10, 9, 11, 5, 6, 13, 19, 26, 14, 15, 18, 23, 24, 25, 8, 7, 12, 16, 21, 20}

#define ZERO_CNT 26
#define ZERO {2, 3, 4, 17, 27, 22, 10, 9, 11, 5, 6, 13, 19, 26, 14, 15, 18, 23, 24, 25, 8, 7, 12, 16, 21, 20}

#define BUFFER_MAX 3
#define DIRECTION_MAX 35
#define VALUE_MAX 30

#define PATH_SYS_EXPORT "/sys/class/gpio/export"
#define PATH_SYS_UNEXPORT "/sys/class/gpio/unexport"

#ifdef PHP_WIN32
#	define PHP_GPIO_EXTENSION_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_GPIO_EXTENSION_API __attribute__ ((visibility("default")))
#else
#	define PHP_GPIO_EXTENSION_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(gpio_extension)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(gpio_extension)
*/

/* Always refer to the globals in your function as GPIO_EXTENSION_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define GPIO_EXTENSION_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(gpio_extension, v)

#if defined(ZTS) && defined(COMPILE_DL_GPIO_EXTENSION)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_GPIO_EXTENSION_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
