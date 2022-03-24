#include "kilo_rand_lib.h"
#include "kilob_common.h"
#include "kilob_random_walk.h"
#include "kilob_tracking.h"
#include "kilolib.h"
#include "utils.h"

#define DURATION_DELAY 100

enum { COLOR_TRANSMIT, COLOR_RECEIVE };//states for the kilobot

tracking_user_data_t tracking_data;

volatile uint8_t flag;
volatile uint8_t color;
volatile uint8_t state;

void diffusion_color() {//randomly chooses a color among red,blue,green and yellow and stores into the next tracking message
  double p = g_ran_uniform();
  if (p < 0.25) {
    set_color(RGB(1, 0, 0));
    tracking_data.byte[2] = 0b001;
  } else if (p < 0.50) {
    set_color(RGB(0, 1, 0));
    tracking_data.byte[2] = 0b010;
  } else if (p < 0.75) {
    set_color(RGB(0, 0, 1));
    tracking_data.byte[2] = 0b100;
  } else {
    set_color(RGB(1, 1, 0));
    tracking_data.byte[2] = 0b011;
  }
}

void ir_rx(IR_message_t *m, distance_measurement_t *d) {
  if (m->type == 0x22) {
    flag = 1;
    color = m->data[0];//stores the color from the message
    state = COLOR_RECEIVE;
  }
  if (m->type == 0x23 && state == COLOR_RECEIVE) {//when the message is received, changes state to COLOR_DIFFUSION
    state = COLOR_TRANSMIT;
    diffusion_color();
  }
}

void setup() {
  kilob_tracking_init();//initializes tracking messages
  g_ran_set_seed_uint32(0);//set seed (for diffusion_color)
  tracking_data.byte[0] = kilo_uid;//kilobot id is sent in tracking messages
  for (unsigned i = 1; i < TRACKING_MSG_USER_DATA; ++i) {
    tracking_data.byte[i] = 0;//initialize the rest of the tracking messages data to 0
  }
  state = COLOR_RECEIVE;//initial state is set to COLOR_RECEIVE
}

void loop() {
  switch (state) {
  case COLOR_RECEIVE://if in COLOR_RECEIVE state, the LED of the Kilobot changes to the color received from Cells' messages
    if (flag) {
      switch (color) {
      case 0b001:
        set_color(RGB(1, 0, 0));
        break;
      case 0b010:
        set_color(RGB(0, 1, 0));
        break;
      case 0b100:
        set_color(RGB(0, 0, 1));
        break;
      case 0b011:
        set_color(RGB(1, 1, 0));
        break;
      default:
        goto END;
      }
      delay(DURATION_DELAY);
    END:
      flag = 0;
    }
    break;

  case COLOR_TRANSMIT:
    kilob_tracking(&tracking_data);//send a tracking message. A color is stored in the message from diffusion_color(). A cell receiving the message will change its LEDs to the color.
    //delay(DURATION_DELAY);
    break;
  }
}

int main() {
  kilo_init();

  utils_init();
  utils_message_rx = ir_rx;

  kilo_start(setup, loop);

  return 0;
}
