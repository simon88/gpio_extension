--TEST--
Check for gpio_extension presence
--SKIPIF--
<?php if (!extension_loaded("gpio_extension")) print "skip"; ?>
--FILE--
<?php 
echo GPIO_IN."\n";
echo GPIO_OUT."\n";
echo GPIO_LOW."\n";
echo GPIO_HIGH."\n";
echo "version\n";
echo ZERO."\n";
echo B_REV_1."\n";
echo B_REV_2."\n";
echo B_PLUS."\n";
?>
--EXPECT--
0
1
0
1
version
ZERO
B_REV_1
B_REV_2
B_PLUS