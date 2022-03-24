#include <stdlib.h>

#include "kilolib.h"
#include "utils.h"
#include <math.h>
#include "kilob_wall_avoidance.h"
#include "kilob_tracking.h"
#include "kilo_rand_lib.h"
#include "../communication.h"
#include "kilob_gradient.h"
#include "kilob_messaging.h"
#include "kilob_random_walk.h"



typedef enum{
  INIT = 0x00,
  GRADIENT_TO_NEST = 0x01,
  GRADIENT_TO_SITE_INIT = 0x02,
  GRADIENT_TO_SITE = 0x03,
  NEST_WALK = 0x04,
  SITEA = 0x05,
  SITEB = 0x6,
  SITE_WALK = 0x07
} kilobot_state_t;

kilobot_state_t state = INIT;


tracking_user_data_t tracking_data;

double check_period = 5;
double turn_mu = 3;
double turn_sigma = 0.5;
double fw_mu = 10;
double fw_sigma = 1;
double stuck_timeout = 20;

uint8_t blacklist[36];

uint8_t pheroA = 0;
uint8_t pheroB = 0;

uint8_t siteA = 0;
uint8_t siteB = 0;

uint8_t lastSite = 2; // 0 siteA, 1 siteB
uint8_t wall_avoidance = 0;

uint8_t follow_trail = 0;
uint32_t follow_trail_timer;
uint32_t follow_trail_period = MS_TO_TICKS(10000);

uint32_t nest_walk_timer;
uint32_t nest_walk_period = MS_TO_TICKS(60000);

uint8_t site_walk_set = 0;
uint32_t site_walk_timer;
uint32_t site_walk_period = MS_TO_TICKS(60000);

uint32_t phero_timer;
uint32_t phero_period = MS_TO_TICKS(300);

IR_message_t* msg;


void drop_pheromone()
{
  if(kilo_ticks < phero_timer)
  {
    return;
  }
  if((msg = kilob_message_send()) != NULL) {
          msg->type = MESSAGE_PHEROMONE_DROP;
          msg->data[0] = kilo_uid;
    msg->data[1] = lastSite;
        }
        phero_timer = kilo_ticks + phero_period;
}


void on_escape_gradient_site(){}

void on_escape_wall(){}

void on_escape_gradient_nest(){}

void inline reset_gradient_tracking()
{
  kilob_gradient_tracking_reset();
  kilob_gradient_tracking_config_add(3, kilob_gradient_tracking_decreasing, on_escape_wall, 1);
}


void message_rx(IR_message_t *m, distance_measurement_t *d)
{
    if(m->type == MESSAGE_GRADIENT)
    {
      if(state == INIT)
      {
        state = GRADIENT_TO_SITE_INIT;
      }
      if(state == GRADIENT_TO_NEST && m->data[0] == 1)
        {
          if(lastSite == 0)
          {
          siteA++;
          }
          if(lastSite == 1)
          {
          siteB++;
          }
          state = NEST_WALK;
          nest_walk_timer = kilo_ticks + nest_walk_period;
        }
      
      if(state == NEST_WALK && m->data[0] == 1)
      {
        if(blacklist[m->data[7]-1] == 1)
        {
          return;
        }
        else
        {
          pheroA += m->data[5];
          pheroB += m->data[6];
          blacklist[m->data[7]-1] = 1;
        }
      }
      
    if(m->data[3] > 0)
    {
   wall_avoidance = 1;
    }


    if(m->data[3] == 0)
    {
    wall_avoidance = 0;
    }

    if((state == SITEA && m->data[2] < 3) || (state == SITEB && m->data[2] > 10))
    {
      state = GRADIENT_TO_SITE_INIT;
    } 

    if((state == GRADIENT_TO_SITE || state == SITEA || state == SITEB) && (m->data[0] == 2 || m->data[0] == 3))
    {
      site_walk_timer = kilo_ticks + site_walk_period;
      reset_gradient_tracking();
      kilob_gradient_tracking_config_add(7, kilob_gradient_tracking_decreasing, on_escape_gradient_nest, 0);
      state = SITE_WALK;
      set_color(RGB(0,1,0));
      pheroA = 0;
      pheroB = 0;
      for(int i = 0; i < 36; ++i)
      {
        blacklist[i] = 0;
      }
      if(m->data[0] == 2)
      {
        lastSite = 0;
      }
      if(m->data[0] == 3)
      {
        lastSite = 1;
      }
    }
    if(state == GRADIENT_TO_SITE && m->data[5] > 0)
    {
      follow_trail = 1;
      follow_trail_timer = kilo_ticks + follow_trail_period;
    }
      
      kilob_gradient_tracking_message_received(m, d);
    }
}



void setup() {
    // put your setup code here, will be run once at the beginning
  g_ran_set_seed_uint32(0);

  kilob_tracking_init();
  tracking_data.byte[0] = kilo_uid;
  tracking_data.byte[1] = state;

  kilob_gradient_tracking_init(check_period, turn_mu,
    turn_sigma, turn_mu, turn_sigma,
    stuck_timeout);
  reset_gradient_tracking();

  kilob_random_walk_init(RW_FORWARD_MU, RW_FORWARD_SIGMA,
  RW_ROTATION_CONST, RW_ROTATION_SPAN);
  phero_timer = kilo_ticks;
}

void loop()
{
  if(wall_avoidance)
  {
   kilob_gradient_tracking();
  }
  else
  {
  switch(state)
  {   
  case GRADIENT_TO_NEST:
      kilob_gradient_tracking();
      drop_pheromone();
  break;
      
  case SITEA:
    kilob_gradient_tracking();
  break;
  
  case SITEB:
   kilob_gradient_tracking();
  break;
  
  case GRADIENT_TO_SITE_INIT:
    set_color(RGB(1,1,0));
    reset_gradient_tracking();
    kilob_gradient_tracking_config_add(4, kilob_gradient_tracking_increasing, on_escape_gradient_site, 0);
    state = GRADIENT_TO_SITE;
  break;

  case GRADIENT_TO_SITE:
    if(kilo_ticks > follow_trail_timer)
    {
      follow_trail = 0;
    }
    if(follow_trail)
    {
      kilob_gradient_tracking();
    } 
    else
    {
      kilob_random_walk();
    }
  break;
  
  case SITE_WALK:
    if(kilo_ticks > site_walk_timer)
    {
      state = GRADIENT_TO_NEST;
      reset_gradient_tracking();
      kilob_gradient_tracking_config_add(1, kilob_gradient_tracking_decreasing, on_escape_gradient_nest, 0);
    }
  kilob_gradient_tracking();
  drop_pheromone();
  break;
  
  case NEST_WALK:
    if(kilo_ticks > nest_walk_timer)
    {
      set_color(RGB(1,1,0));
      reset_gradient_tracking();
     if(pheroA > pheroB)
     {
      kilob_gradient_tracking_config_add(2, kilob_gradient_tracking_decreasing, on_escape_wall, 0);
      state = SITEA;
     }
     else if(pheroA < pheroB)
     {
      kilob_gradient_tracking_config_add(2, kilob_gradient_tracking_increasing, on_escape_wall, 0);
      state = SITEB;
     }
     else if(g_ran_uniform() < 0.5)
     {
      kilob_gradient_tracking_config_add(2, kilob_gradient_tracking_decreasing, on_escape_wall, 0);
      state = SITEA;
     }
     else
     {
      kilob_gradient_tracking_config_add(2, kilob_gradient_tracking_increasing, on_escape_wall, 0);
      state = SITEB;
     }
    }
    kilob_gradient_tracking();
    drop_pheromone();
  break;

  default:
  break;
  }
}
  tracking_data.byte[1] = state;
  tracking_data.byte[2] = siteA;
  tracking_data.byte[3] = siteB;
  tracking_data.byte[4] = pheroA;
  tracking_data.byte[5] = pheroB;
  tracking_data.byte[6] = lastSite;
  kilob_tracking(&tracking_data);
}

int main() {
    kilo_init();
    utils_init();

    utils_message_rx = message_rx; // register IR reception callback
  
    kilo_start(setup, loop);

    return 0;
}
