--TEST--
Check gpio constructor
--SKIPIF--
<?php if (!extension_loaded("gpio_extension")) print "skip"; ?>
--FILE--
<?php 
$list_pin = array(20 => GPIO_IN, 21 => GPIO_OUT);
$gpio = new Gpio(ZERO, $list_pin);
var_dump($gpio);

echo $gpio->getVersion()."\n";

echo "bad gpio\n";

$list_pin = array(200 => GPIO_IN, 21 => GPIO_OUT);
$gpio = new Gpio(ZERO, $list_pin);

?>
--EXPECTF--
object(Gpio)#%d (%d) {
  ["version"]=>
  string(%d) "%s"
  ["pins"]=>
  array(2) {
    [20]=>
    int(0)
    [21]=>
    int(1)
  }
}
ZERO
bad gpio

Fatal error: Pin %d is not valid for %s in %s on line %d