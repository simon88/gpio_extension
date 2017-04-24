dnl $Id$
dnl config.m4 for extension gpio_extension

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(gpio_extension, for gpio_extension support,
dnl Make sure that the comment is aligned:
dnl [  --with-gpio_extension             Include gpio_extension support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(gpio_extension, whether to enable gpio_extension support,
Make sure that the comment is aligned:
[  --enable-gpio_extension           Enable gpio_extension support])

if test "$PHP_GPIO_EXTENSION" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-gpio_extension -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/gpio_extension.h"  # you most likely want to change this
  dnl if test -r $PHP_GPIO_EXTENSION/$SEARCH_FOR; then # path given as parameter
  dnl   GPIO_EXTENSION_DIR=$PHP_GPIO_EXTENSION
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for gpio_extension files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       GPIO_EXTENSION_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$GPIO_EXTENSION_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the gpio_extension distribution])
  dnl fi

  dnl # --with-gpio_extension -> add include path
  dnl PHP_ADD_INCLUDE($GPIO_EXTENSION_DIR/include)

  dnl # --with-gpio_extension -> check for lib and symbol presence
  dnl LIBNAME=gpio_extension # you may want to change this
  dnl LIBSYMBOL=gpio_extension # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $GPIO_EXTENSION_DIR/$PHP_LIBDIR, GPIO_EXTENSION_SHARED_LIBADD)
        AC_DEFINE(HAVE_GPIO_EXTENSIONLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong gpio_extension lib version or lib not found])
  dnl ],[
  dnl   -L$GPIO_EXTENSION_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(GPIO_EXTENSION_SHARED_LIBADD)

  PHP_NEW_EXTENSION(gpio_extension, gpio_extension.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
