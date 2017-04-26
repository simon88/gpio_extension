# GPIO extension
`PHP 7` extension to use directly `GPIO` in `PHP` script by sysfs. This extension work only on `Linux`.

## What you need ?
First you need PHP 7. Downlad last source.
```
$ sudo apt-get install re2c
$ sudo apt-get install bison

$ git clone http://git.php.net/repository/php-src.git
$ cd php-src
$ ./buildconf
$ ./configure --disable-all --enable-debug --enable-maintainer-zts
$ make -j4
$ sudo make install
```

Test if `PHP7` is install
```
php -v
```

## How compile
Now you have `PHP7` you can compile `gpio extension`
```
$ cd gpio_extension
$ phpize
$ configure --enable-gpio_extension
$ make
$ make test
```

## How to use extension
Gpio extension contains some constant :
```
//Gpio state
GPIO_IN
GPIO_OUT

GPIO_LOW
GPIO_HIGH

//Raspberry version
B_REV_1
B_REV_2
B_PLUS
ZERO

```
To test gpio extension you can add LED with 330 ohm resistor on random GPIO.
Now extension is ready to use. To use it create new empty php file and add this :
```
<?php
//Create an array for GPIO to use (use GPIO number)
$list_pin = array(
  20 => GPIO_IN,
  21 => GPIO_OUT
);

$gpio = new Gpio(ZERO, list_pin);

//Show raspberry verison select
echo $gpio->getVersion();

//List all valid gpio for version
var_dump($gpio->getAllValidPin());


//Export all gpio pin set in construct
$gpio->export();

//Set gpio direction
$gpio->setDirection();

//Read value of specifi GPIO
echo "value gpio20 ".$gpio->gpioRead(20);

//Write on specifi gpio
$gpio->gpioWrite(20, GPIO_HIGH);

//if you have LED and resistor on GPIO20 you can do that :
for($i=0 $i<10;$i++){
  $gpio->gpioWrite(20, GPIO_HIGH);
  sleep(1);
  $gpio->gpioWrite(20, GPIO_LOW);
}

//Unexport all gpio pin
$gpio->unexport();
```

Now you can run this file and enjoy
```
php -dextension=modules/gpio_extension.so -f file.php
```
