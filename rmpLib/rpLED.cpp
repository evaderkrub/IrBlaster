
#include "rpLED.h"
#include "hardware/gpio.h"

const int PI_PICO_LED_PIN = 25;


rpLED::rpLED()
{
    iIOPin = PI_PICO_LED_PIN; // use default pi pico LED
}
        
rpLED::rpLED(int iPin)
{
    iIOPin = iPin;
}

void rpLED::init()
{
    gpio_init(iIOPin);
    gpio_set_dir(iIOPin,GPIO_OUT);
    gpio_set_drive_strength(iIOPin, GPIO_DRIVE_STRENGTH_12MA);
    bHasBeenInit = true;
}

void rpLED::on()
{
    if (!bHasBeenInit) init();
    gpio_put(iIOPin, 1);
}
void rpLED::off()
{
     if (!bHasBeenInit) init();
    gpio_put(iIOPin, 0);
}

void rpLED::toggle()
{
     if (!bHasBeenInit) init();
     gpio_put(iIOPin, !gpio_get(iIOPin));
}


