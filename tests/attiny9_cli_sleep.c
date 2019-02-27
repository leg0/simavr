#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "avr_mcu_section.h"
AVR_MCU(F_CPU, "attiny9");

int main()
{
    cli();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_mode(); // this should cause the simulator to stop
}
