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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_gpio_extension.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

/* If you declare any globals in php_gpio_extension.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(gpio_extension)
*/

/* True global resources - no need for thread safety here */
static int le_gpio_extension;
static uint32_t valid_pins[B_REV_2_CNT] = {0};

zend_class_entry *gpio_ce;


/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(gpio_extension)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(gpio_extension)
{
#if defined(COMPILE_DL_GPIO_EXTENSION) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(gpio_extension)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(gpio_extension)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "gpio_extension support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ Check if pin is valide depending on the raspberry version
 */
static bool checkValidePin(const int gpio_pin, int count_for)
{
	int index;
	bool valid_or_not = false;

    for (index = 0; index < count_for; index++){
        if (gpio_pin == valid_pins[index]){
            valid_or_not = true;
            break;
        }
    }

    return valid_or_not;
}
/* }}} */

/* {{{ Initialize for raspberry version valid_pins
 */
static void getAllValidPinForVersion(char *version)
{
    if (strcmp(version, "B_REV_2") == 0){
		const uint32_t all_valid_pins[B_REV_2_CNT] = B_REV_2;
		memcpy(valid_pins, all_valid_pins, sizeof(all_valid_pins));

	}else if (strcmp(version, "B_REV_1") == 0){
		const uint32_t all_valid_pins[B_REV_1_CNT] = B_REV_1;
		memcpy(valid_pins, all_valid_pins, sizeof(all_valid_pins));

	}else if (strcmp(version, "B_PLUS") == 0){
		const uint32_t all_valid_pins[B_PLUS_CNT] = B_PLUS;
		memcpy(valid_pins, all_valid_pins, sizeof(all_valid_pins));

	}else if (strcmp(version, "ZERO") == 0){
		const uint32_t all_valid_pins[ZERO_CNT] = ZERO;
		memcpy(valid_pins, all_valid_pins, sizeof(all_valid_pins));
	}
}
/* }}} */

/* {{{ Check for one pin and one version if arg pin is valid
 */
static bool checkPinForVersion(int rasp_pin, char *version)
{
	bool valid_pin = false;

	if (strcmp(version, "B_REV_2") == 0){
		valid_pin = checkValidePin(rasp_pin, B_REV_2_CNT);

	}else if (strcmp(version, "B_REV_1") == 0){
		valid_pin = checkValidePin(rasp_pin, B_REV_1_CNT);

	}else if (strcmp(version, "B_PLUS") == 0){
		valid_pin = checkValidePin(rasp_pin, B_PLUS_CNT);

	}else if (strcmp(version, "ZERO") == 0){
		valid_pin = checkValidePin(rasp_pin, ZERO_CNT);

	}

	return valid_pin;
}
/* }}} */

static int getCountPin(char *version)
{
	int count_for;

  	if (strcmp(version, "B_REV_2") == 0){
		count_for = B_REV_2_CNT;

	}else if (strcmp(version, "B_REV_1") == 0){
		count_for = B_REV_1_CNT;

	}else if (strcmp(version, "B_PLUS") == 0){
		count_for = B_PLUS_CNT;

	}else if (strcmp(version, "ZERO") == 0){
		count_for = ZERO_CNT;

	}

	return count_for;
}

/* {{{ Function how try to export gpio pin set in __construct
 */
static int export(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;
 
	fd = open(PATH_SYS_EXPORT, O_WRONLY);
	if (fd == -1) {
		return -1;
	}
 
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);

	return 0;
}
/* }}} */

/* {{{ Function how try to unexport gpio pin set in __construct
 */
static int unexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;
 
	fd = open(PATH_SYS_UNEXPORT, O_WRONLY);
	if (fd == -1) {
		return -1;
	}
 
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);

	return 0;
}
/* }}} */

/* {{{ Set direction of pin IN or OUT
 */
static int direction(int pin, int dir)
{
	char path[DIRECTION_MAX];
	int fd;
	ssize_t bytes_written;

 	zend_string *str_const_in = zend_string_init("GPIO_IN", strlen("GPIO_IN"), 1);
  	zend_string *str_const_out = zend_string_init("GPIO_OUT", strlen("GPIO_OUT"), 1);

    zval *const_in = zend_get_constant(str_const_in);
    zval *const_out = zend_get_constant(str_const_out);
 
	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (fd == -1) {
		return -1;
	}

	if(dir == Z_LVAL_P(const_in))
		bytes_written = write(fd, "in", 2);
	else if(dir == Z_LVAL_P(const_out))
		bytes_written = write(fd, "out", 3);

 
	if (bytes_written != 2 || bytes_written != 3) {
		return -2;
	}
 
	close(fd);

	return 0;
}
/* }}} */

/* {{{ Try to read value of one pin
 */
static int readPin(int pin)
{
	char path[VALUE_MAX];
	char value_str[3];
	int fd;
 
	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);

	if (fd == -1) {
		return -1;
	}
 
	if ( read(fd, value_str, 3) == -1 ) {
		return -2;
	}
 
	close(fd);
 
	return atoi(value_str);
}

/* {{{ Try to write value on pin
 */
static int writePin(int pin, int value)
{
 
	char path[VALUE_MAX];
	int fd;
 	ssize_t bytes_written;
 	zval val_const_low, val_const_high;

 	zend_string *str_const_low = zend_string_init("GPIO_LOW", strlen("GPIO_LOW"), 1);
  	zend_string *str_const_high = zend_string_init("GPIO_HIGH", strlen("GPIO_HIGH"), 1);

    zval *const_low = zend_get_constant(str_const_low);
    zval *const_high = zend_get_constant(str_const_high);

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		return -1;
	}

	if(value == Z_LVAL_P(const_low))
		bytes_written = write(fd, "0", 1);
	else if(value == Z_LVAL_P(const_high))
		bytes_written = write(fd, "1", 1);

 
	if (bytes_written != 1) {
		return -2;
	}

	close(fd);

	return 0;
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gpio_read, 0, 0, 1)
	ZEND_ARG_INFO(0, pin)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gpio_write, 0, 0, 2)
	ZEND_ARG_INFO(0, pin)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gpio_ctor, 0, 0, 2)
	ZEND_ARG_INFO(0, rasp_version)
    ZEND_ARG_INFO(1, pins)
ZEND_END_ARG_INFO()

/* {{{ Gpio construct
 */
PHP_METHOD(Gpio, __construct) 
{
	zval *list_pin, *result_const;
	zval *obj, *val;
	int count, i;
	zend_string *rasp_version;

	ulong rasp_pin;
	zend_string *str_idx;
	bool valid_pin;

  	zend_string *str_const_in = zend_string_init("GPIO_IN", strlen("GPIO_IN"), 1);
  	zend_string *str_const_out = zend_string_init("GPIO_OUT", strlen("GPIO_OUT"), 1);

    zval *const_in = zend_get_constant(str_const_in);
    zval *const_out = zend_get_constant(str_const_out);

    array_init(return_value);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sa", &rasp_version, &list_pin) == FAILURE) {
        return;
    }

    obj = getThis();

    //check if version is valid
    if(result_const = zend_get_constant(rasp_version) == NULL){
		zend_error(E_ERROR, "Version '%s' isn't available in extension", rasp_version->val);
    }

    zend_update_property_string(gpio_ce, obj, "version", sizeof("version")-1, rasp_version->val);

    //get all valid gpio pin by raspberry version
    getAllValidPinForVersion(rasp_version->val);

    //check if arg array are good format : key -> LONG and value -> STRING 2 value possible 0 and 1
    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(list_pin), rasp_pin, str_idx, val) {

	    if(Z_TYPE_P(val) == IS_STRING)
	    	convert_to_long(val);


	    if(Z_LVAL_P(val) != Z_LVAL_P(const_in) && Z_LVAL_P(val) != Z_LVAL_P(const_out)){
			zend_error(E_ERROR, "Value for GPIO pin just can be 0 or 1");
		}

	    if(str_idx){
	    	zend_error(E_ERROR, "Value key must be int");
    	}

    	//check if pin is valid for raspberry version
    	valid_pin = checkPinForVersion(rasp_pin, rasp_version->val);
    	
    	if(!valid_pin){
			zend_error(E_ERROR, "Pin %d is not valid for %s", rasp_pin, rasp_version->val);
		}

	    //create list with array arg
	    add_index_long(return_value, rasp_pin, Z_LVAL_P(val));

	}ZEND_HASH_FOREACH_END();

	zend_update_property(gpio_ce, obj, "pins", sizeof("pins")-1, return_value TSRMLS_CC);

}

/* {{{ Return raspberry version
 */
PHP_METHOD(Gpio, getVersion)
{
    zval *obj, *version;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    obj = getThis();

    version = zend_read_property(gpio_ce, obj, "version", sizeof("version")-1, 1 TSRMLS_CC, NULL);

    RETURN_ZVAL(version, 1, 0);
}
/* }}} */

/* {{{ Return all valid pin for raspberry
 */
PHP_METHOD(Gpio, getAllValidPin)
{
    zval *obj, *rasp_version;
    int index, count_for;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    obj = getThis();

    rasp_version = zend_read_property(gpio_ce, obj, "version", sizeof("version")-1, 1 TSRMLS_CC, NULL);

    convert_to_string(rasp_version);

    getAllValidPinForVersion(Z_STRVAL_P(rasp_version));

	count_for = getCountPin(Z_STRVAL_P(rasp_version));

    array_init(return_value);

    for (index = 0; index < count_for; index++){
         add_next_index_long(return_value, valid_pins[index]);
    }

}
/* }}} */

/* {{{ Export pin set by user in construct
 */
PHP_METHOD(Gpio, gpioExport) 
{
	int valid_export;
   	zval *obj, *pins, *val;

   	ulong rasp_pin;
	zend_string *str_idx;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    obj = getThis();

    pins = zend_read_property(gpio_ce, obj, "pins", sizeof("pins")-1, 1 TSRMLS_CC, NULL);

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(pins), rasp_pin, str_idx, val) {

    	valid_export = export(rasp_pin);
		if(valid_export == -1){
			zend_error(E_ERROR, "Failed to open export for writing!");
		}
	}ZEND_HASH_FOREACH_END();
}
/* }}} */

PHP_METHOD(Gpio, gpioUnexport)
{
	int valid_unexport;
   	zval *obj, *pins, *val;

   	ulong rasp_pin;
	zend_string *str_idx;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    obj = getThis();

    pins = zend_read_property(gpio_ce, obj, "pins", sizeof("pins")-1, 1 TSRMLS_CC, NULL);

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(pins), rasp_pin, str_idx, val) {

    	valid_unexport = unexport(rasp_pin);
		if(valid_unexport == -1){
			zend_error(E_ERROR, "Failed to open unexport for writing!");
		}
	}ZEND_HASH_FOREACH_END();
}

/* {{{ Try to set direction off all pin set in constructor
 */
PHP_METHOD(Gpio, gpioDirection)
{
	int result;
   	zval *obj, *pins, *val;

   	ulong rasp_pin;
	zend_string *str_idx;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    obj = getThis();

    pins = zend_read_property(gpio_ce, obj, "pins", sizeof("pins")-1, 1 TSRMLS_CC, NULL);

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(pins), rasp_pin, str_idx, val) {

		result = direction(rasp_pin, Z_LVAL_P(val));
		if(result == -1)
			zend_error(E_ERROR, "Failed to open gpio direction for writing!");
		else if(result == -2)
			zend_error(E_ERROR, "Failed to set direction!");

	}ZEND_HASH_FOREACH_END();
}

/* {{{ Try to read value of pin
 */
PHP_METHOD(Gpio, gpioRead)
{
	zend_long pin;
	int result;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pin) == FAILURE) {
        return;
    }

    result = readPin(pin);

    if(result == -1)
    	zend_error(E_ERROR, "Failed to open gpio value for reading!");
    else if(result == -2)
    	zend_error(E_WARNING, "Failed to read value of %d!", pin);
    else
	 	RETVAL_LONG(result);
}

/* {{{ Try to write value on gpio pin
 */
PHP_METHOD(Gpio, gpioWrite)
{
	zend_long pin, value;
	int result;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pin, &value) == FAILURE) {
        return;
    }

    result = writePin(pin, value);

  	if(result == -1)
    	zend_error(E_ERROR, "Failed to open gpio value for writing!");
    else if(result == -2)
    	zend_error(E_WARNING, "Failed to write value  %d on pin %d!", value, pin);
    else
	 	RETVAL_BOOL(1);

}

/* {{{ gpio_extension_functions[]
 *
 * Every user visible function must have an entry in gpio_extension_functions[].
 */
const zend_function_entry gpio_extension_functions[] = {
	PHP_ME(Gpio, __construct, arginfo_gpio_ctor, ZEND_ACC_PUBLIC)
	PHP_ME(Gpio, getVersion, arginfo_void, ZEND_ACC_PUBLIC)
	PHP_ME(Gpio, getAllValidPin, arginfo_void, ZEND_ACC_PUBLIC)
	PHP_ME(Gpio, gpioExport, arginfo_void, ZEND_ACC_PUBLIC)
	PHP_ME(Gpio, gpioUnexport, arginfo_void, ZEND_ACC_PUBLIC)
	PHP_ME(Gpio, gpioDirection, arginfo_void, ZEND_ACC_PUBLIC)
	PHP_ME(Gpio, gpioRead, arginfo_gpio_read, ZEND_ACC_PUBLIC)
	PHP_ME(Gpio, gpioWrite, arginfo_gpio_write, ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in gpio_extension_functions[] */
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(gpio_extension)
{
    zend_class_entry tmp_ce;

    zval *cd = (zval*)malloc(sizeof(zval));

    INIT_CLASS_ENTRY(tmp_ce, "Gpio", gpio_extension_functions);

    gpio_ce = zend_register_internal_class(&tmp_ce TSRMLS_CC);

    //constant
    REGISTER_STRING_CONSTANT("B_REV_1", "B_REV_1", CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("B_REV_2", "B_REV_2", CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("B_PLUS", "B_PLUS", CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("ZERO", "ZERO", CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("GPIO_IN", 0, CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("GPIO_OUT", 1, CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("GPIO_LOW", 0, CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("GPIO_HIGH", 1, CONST_PERSISTENT);

    return SUCCESS;
}
/* }}} */

/* {{{ gpio_extension_module_entry
 */
zend_module_entry gpio_extension_module_entry = {
	STANDARD_MODULE_HEADER,
	"gpio_extension",
	gpio_extension_functions,
	PHP_MINIT(gpio_extension),
	PHP_MSHUTDOWN(gpio_extension),
	PHP_RINIT(gpio_extension),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(gpio_extension),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(gpio_extension),
	PHP_GPIO_EXTENSION_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_GPIO_EXTENSION
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(gpio_extension)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
