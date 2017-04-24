--TEST--
Check valid pin
--SKIPIF--
<?php if (!extension_loaded("gpio_extension")) print "skip"; ?>
--FILE--
<?php 
$list_pin = array(20 => GPIO_IN, 21 => GPIO_OUT);
$gpio = new Gpio(ZERO, $list_pin);

var_dump($gpio->getAllValidPin());

$list_pin = array(3 => GPIO_IN, 5 => GPIO_OUT);
$gpio = new Gpio(B_REV_2, $list_pin);

var_dump($gpio->getAllValidPin());

?>
--EXPECTF--
array(%d) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(17)
  [4]=>
  int(27)
  [5]=>
  int(22)
  [6]=>
  int(10)
  [7]=>
  int(9)
  [8]=>
  int(11)
  [9]=>
  int(5)
  [10]=>
  int(6)
  [11]=>
  int(13)
  [12]=>
  int(19)
  [13]=>
  int(26)
  [14]=>
  int(14)
  [15]=>
  int(15)
  [16]=>
  int(18)
  [17]=>
  int(23)
  [18]=>
  int(24)
  [19]=>
  int(25)
  [20]=>
  int(8)
  [21]=>
  int(7)
  [22]=>
  int(12)
  [23]=>
  int(16)
  [24]=>
  int(21)
  [25]=>
  int(20)
}
array(%d) {
  [0]=>
  int(3)
  [1]=>
  int(5)
  [2]=>
  int(7)
  [3]=>
  int(11)
  [4]=>
  int(13)
  [5]=>
  int(15)
  [6]=>
  int(19)
  [7]=>
  int(21)
  [8]=>
  int(23)
  [9]=>
  int(8)
  [10]=>
  int(10)
  [11]=>
  int(12)
  [12]=>
  int(16)
  [13]=>
  int(18)
  [14]=>
  int(22)
  [15]=>
  int(24)
  [16]=>
  int(26)
}

