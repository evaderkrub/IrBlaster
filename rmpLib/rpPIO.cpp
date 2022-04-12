
#include "rpPIO.h"

#include "hardware/pio.h"


unsigned int PIO_encode_jmp_always(unsigned char bDelaySideSet, unsigned char bAddress)
{
    unsigned int iTemp = 0x0; // jump command

    // condition is ALWAYS (000)

    iTemp |= (0x1f & bAddress);

    return iTemp;
}

unsigned int PIO_encode_set_pins_direction(unsigned char bDelaySideSet, unsigned char bDirection)
{
    unsigned int iTemp = 0xE000; // set command

    // TODO IMPLEMENT DELAY SIDE SET

    // destination is PINSDIR (100)
    iTemp |= 0x80;

    iTemp |= (0x1f & bDirection);

    return iTemp;
}

unsigned int PIO_encode_set_pins(unsigned char bDelaySideSet, unsigned char bData)
{
    unsigned int iTemp = 0xE000; // set command

    // TODO IMPLEMENT DELAY SIDE SET

    // destination is PINS (000)
    iTemp |= (0x1f & bData);

    return iTemp;
}


void init()
{

    PIO pio = pio0;

pio->instr_mem[0] = PIO_encode_set_pins_direction(0,1);
    pio->instr_mem[1] = PIO_encode_set_pins(0,1);
    pio->instr_mem[2] = PIO_encode_set_pins(0,0);
    pio->instr_mem[3] = PIO_encode_jmp_always(0,1);
    
    pio->sm[0].pinctrl =
        (1 << PIO_SM0_PINCTRL_SET_COUNT_LSB) |
        (0 << PIO_SM0_PINCTRL_SET_BASE_LSB);

   // gpio_set_function(0, GPIO_FUNC_PIO0);
   // pio_gpio_init(pio,0);

    // start the first state machine
  //  hw_set_bits(&pio->ctrl, 1 << (PIO_CTRL_SM_ENABLE_LSB + 0));

}
