# GPIO extension

PHP 7 extension for direct control over `GPIO` pins, using syfs.
This extension works on Linux only.

## Getting started

First you need PHP 7. Downlad latest sources.

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

Make sure that PHP 7 is present in your system:

```
php -v
```

## Compiling
Now, when you have PHP 7, you can compile `gpio extension`

```
$ cd gpio_extension
$ phpize
$ configure --enable-gpio_extension
$ make
$ make test
```

## Usage

Gpio extension defines some utility constants:

```c
// GPIO state
GPIO_IN
GPIO_OUT

GPIO_LOW
GPIO_HIGH

// Raspberry version
B_REV_1
B_REV_2
B_PLUS
ZERO
```

To test the extension you can plug in LED with 330 ohm resistor on random GPIO.
Now extension is ready to use. To use it create new empty PHP file and add this:

```php
<?php
// Create an array for GPIO to use (use GPIO number)
$list_pin = array(
  20 => GPIO_IN,
  21 => GPIO_OUT
);

$gpio = new Gpio(ZERO, $list_pin);

// Show Raspberry verison
echo $gpio->getVersion();

// List all valid GPIO pins for this version
var_dump($gpio->getAllValidPin());

// Export all GPIO pins set in construct
$gpio->export();

//  Set gpio direction
$gpio->setDirection();

// Read value of specific GPIO pin
echo "Value of gpio20 is ".$gpio->gpioRead(20);

// Write to specific pin
$gpio->gpioWrite(20, GPIO_HIGH);

// If you have LED and resistor on GPIO20, you can do that :
for ($i=0 $i<10;$i++) {
    $gpio->gpioWrite(20, GPIO_HIGH);
    sleep(1);
    $gpio->gpioWrite(20, GPIO_LOW);
}

// Unexport all GPIO pins
$gpio->unexport();
```

To run this file you need to either add valid `extension=` entry in your `php.ini`
or use `-d` commandline option, like this:

```
php -dextension=modules/gpio_extension.so -f file.php
```
