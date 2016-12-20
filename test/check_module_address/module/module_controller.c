/** @file CellMain.c */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>  // required if you need delay macro

#include <CAN.h>
#include <debug.h>
#include <module.h>
#include <moduleIR.h>
#include <moduleLED.h>


static const uint16_t X_MAX    = 10;
static const uint16_t Y_MAX    = 20;
static const uint32_t INTERVAL = 3000;

volatile uint16_t x, y;


void setup()
{
    x = y = 0;
}


void loop()
{
    uint8_t flag = 0;
    flag |= (module_uid_x_coord == x ? 1 : 0);  // if x == module_uid_x_coord then set 0b01
    flag |= (module_uid_y_coord == y ? 2 : 0);  // if y == module_uid_y_coord then set 0b10
    // 'flag' can be 0b00, 0b01, 0b10 or 0b11 here
    switch(flag) {
        case 1:     // row x
            set_all_LEDs(BLUE);    break;
        case 2:     // column y
            set_all_LEDs(GREEN);   break;
        case 3:     // module x-y
            set_all_LEDs(RED);     break;
        default:    // others
            set_all_LEDs(LED_OFF);
    }

    // go back to first module if there is no next module, otherwise go to next module
    y = ++y >= Y_MAX ? 0 : y;
    x = y == 0 ? ++x >= X_MAX ? 0 : x : x;

    _delay_ms(INTERVAL);
}


int main()
{
    module_init();
    module_start(setup, loop);
    return 0;
}
