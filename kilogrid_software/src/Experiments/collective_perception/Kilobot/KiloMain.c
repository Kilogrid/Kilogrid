#include <stdlib.h>

#include <kilolib.h>
#include <ringbuffer.h>
#include <utils.h>
#include <kilogrid.h>

#include <kilob_common.h>
#include <kilob_tracking.h>
#include <kilob_random_walk.h>
#include <kilob_wall_avoidance.h>
#include <kilob_cdms.h>
#include <kilob_messaging.h>

#include "../Communication.h"
#include "config.h"


// Experimental configs
#define MAJORITY_RULE
#define INITIAL_OPINION                 kilob_cdms_random_opinion()  // for demo.
//#define INITIAL_OPINION                 4

// #define TEST_WELL_MIXED
// #define DEBUG


// typedefs
typedef enum {
    STOP,
    RANDOM_WALK,
    WALL_AVOIDANCE
} kilobot_action_t;


typedef enum {
    INITIAL,
    EXPLORATION,
    DISSEMINATION,
    DICISION_MAKING,
    ERROR_STATE = 0xEE
} kilobot_state_t;


// Action
kilobot_action_t action;

// State
kilobot_state_t state;

// Opinion
opinion_t opinion;

// Timer
volatile float    due_time;
volatile uint16_t blink_counter;
volatile float    next_dissemination;
volatile uint8_t  force_disseminate;

static float get_current_sec();
static float get_duration_sec(kilobot_state_t);

// Quality
volatile uint32_t duration_explorate;
volatile uint32_t duration_match;
volatile float quality;

// Tracking
static tracking_user_data_t tracking_data;
static void send_state_tracking();

#ifdef TEST_WELL_MIXED
static tracking_user_data_t neighborhood_data;
volatile uint8_t waiting_neighborhood_data_sent = 0;

static inline void send_neighborhood_tracking()
{
    IR_message_t *msg = kilob_message_send();
    if(msg == NULL) return;
    msg->type = MESSAGE_FOR_TRACKING;
    for(uint8_t i = 0; i < TRACKING_MSG_USER_DATA; ++i) {
        msg->data[i] = neighborhood_data.byte[i];
    }
    waiting_neighborhood_data_sent = 0;
}

static inline unsigned min(unsigned a, unsigned b)
{
    return a < b ? a : b;
}
#endif


// IR message
static void wakeup();
static void wall_detect(IR_message_t* m, distance_measurement_t* d);
static void detect_cell_state(IR_message_t* m);
static void collect_opinion(IR_message_t* m);

void IR_rx(IR_message_t* m, distance_measurement_t* d)
{
    if(state == INITIAL) {
        // Wake up kilobot if cell is running
        switch(m->type) {
            case MESSAGE_ADVERTISE_CELL_STATE:
                wakeup();
				break;
            default:
                return;
        }
    }

    switch(m->type) {
        case MESSAGE_ADVERTISE_CELL_STATE:
            if(m->data[INDEX_WALL_FLAG] == 1) {
                wall_detect(m, d);
            }
            if(state == EXPLORATION) {
                detect_cell_state(m);
            }
            break;
        case MESSAGE_ADVERTISE_KILOBOT_OPINION:
            if(state == DISSEMINATION) {
                collect_opinion(m);
            }
            break;
    }
}


// Main
void setup()
{
    g_ran_set_seed_uint32(0);
    kilob_tracking_init();
    kilob_random_walk_init(RANDOM_WALK_FORWARD_MEAN, RANDOM_WALK_FORWARD_SIGMA,
                           RANDOM_WALK_ROT_CONST, RANDOM_WALK_ROT_SPAN);
    kilob_wall_avoidance_init();
    kilob_cdms_init(NUMBER_OF_OPINIONS, GROUP_SIZE);

    action  = STOP;
    state   = INITIAL;
    opinion = INITIAL_OPINION;
    tracking_data.byte[0] = kilo_uid;
    for(int i = 1; i < TRACKING_MSG_USER_DATA; ++i) {
        tracking_data.byte[i] = 0;
    }
#ifdef TEST_WELL_MIXED
    neighborhood_data.byte[0] = (0x80 | kilo_uid);
#endif
}


static void action_loop();
static void state_loop();
static void send_state_tracking();

void loop() {
    action_loop();
    state_loop();

#ifdef TEST_WELL_MIXED
    if(waiting_neighborhood_data_sent) {
        send_neighborhood_tracking();
    }
#endif
    send_state_tracking();
}


int main() {
    kilo_init();
    utils_init();
    utils_message_rx = IR_rx;

    kilo_start(setup, loop);

    return 0;
}


/******************************************************/
inline static uint8_t color_from_opinion(opinion_t op)
{
    switch(op) {
        case 0:
            return RGB(1,0,0);
        case 1:
            return RGB(0,0,1);
        case 2:
            return RGB(1,1,0);
        case 3:
            return RGB(1,1,1);
        case 4:
            return RGB(0,1,0);
        default:
            return RGB(1,0,1);
    }
}


enum {
    SECTION_KILOUID = 0,
    SECTION_OPINION,
    SECTION_DURATION_MATCH,
    SECTION_DURATION_EXPLORATE
} msg_data_section_t;


inline static void clear_duration_counters()
{
    duration_explorate = duration_match = 0;
}


inline static void wakeup()
{
    state    = EXPLORATION;
    action   = RANDOM_WALK;
    due_time = get_current_sec() + get_duration_sec(EXPLORATION);
    kilob_cdms_clear_buffer();
    clear_duration_counters();
}


inline static void collect_opinion(IR_message_t* m)
{
    uint8_t   id = m->data[SECTION_KILOUID];
    opinion_t op = m->data[SECTION_OPINION];
    float     q  = m->data[SECTION_DURATION_EXPLORATE] != 0
                    ? m->data[SECTION_DURATION_MATCH] / (float)m->data[SECTION_DURATION_EXPLORATE]
                    : 0.0;

    kilob_cdms_collect(id, op, q);
}


inline static void wall_detect(IR_message_t* m, distance_measurement_t* d)
{
    action = WALL_AVOIDANCE;
    kilob_on_wall_detected(d, m);
}


inline static void detect_cell_state(IR_message_t* m)
{
    if(m->type != MESSAGE_ADVERTISE_CELL_STATE || state != EXPLORATION) return;

    duration_explorate += 1;
    duration_match     += (m->data[INDEX_CELL_STATE] == opinion ? 1 : 0);
}


inline static void action_loop()
{
    switch(action) {
        case WALL_AVOIDANCE:
            if(!kilob_wall_avoidance()) {
                action = RANDOM_WALK;
            }
            break;
        case RANDOM_WALK:
            kilob_random_walk();
            break;
        default:
            set_motors(0, 0);
    }
}


inline static uint8_t decision_making()
{
#if   defined(VOTER_MODEL)
    return kilob_cdms_voter_model(opinion);
#elif defined(MAJORITY_RULE)
    return kilob_cdms_majority_rule(opinion);
#elif defined(K_UNANIMITY)
    return kilob_cdms_unanimity_rule(opinion);
#elif defined(DIRECT_COMPARISON)
    return kilob_cdms_direct_comparison(opinion, quality);
#else
#error No decision rule.
#endif
}


static uint8_t disseminate();

inline static void state_loop()
{
    float current_sec = get_current_sec();

    switch(state) {

        case INITIAL:
            set_color(color_from_opinion(opinion));
            return;

        case EXPLORATION:
            set_color(color_from_opinion(opinion));

            if(current_sec > due_time) {
                quality  = (duration_explorate == 0 ? 0.0 : duration_match/(float)duration_explorate);
                due_time = current_sec + g_ran_exponential(1.0/(double)(quality * MEAN_DURATION_OF_DISSEMINATION));
                state    = DISSEMINATION;
                force_disseminate = 1;
            }
            break;

        case DISSEMINATION:
            if(force_disseminate || next_dissemination <= current_sec) {
                force_disseminate  = !disseminate();
                next_dissemination = current_sec + DISSEMINATION_INTERVAL;
            }

            if(current_sec < due_time || kilob_cdms_count_collected_opinions() < GROUP_SIZE) {
                if(blink_counter < BLINK_SPAN) {
                    set_color(BLINK_COLOR);
                    ++blink_counter;
                } else {
                    set_color(color_from_opinion(opinion));
                    ++blink_counter;
                }
                if(blink_counter >= 2 * BLINK_SPAN) {
                    blink_counter = 0;
                }
            } else {
                state    = DICISION_MAKING;
                due_time = 0.0;
            }
            break;

        case DICISION_MAKING:
            opinion  = decision_making();
            state    = EXPLORATION;
            quality  = 0.0;
            due_time = current_sec + g_ran_exponential(1.0/(float)MEAN_DURATION_OF_EXPLORATION);
            clear_duration_counters();
#ifdef TEST_WELL_MIXED
            // user_byte_data[1:6] := neighborhood opinions
            for(unsigned i = 1; i < min(kilob_cdms_count_collected_opinions() + 1, TRACKING_MSG_USER_DATA); ++i) {
                unsigned index        = i - 1;
                opinion_info_t info   = kilob_cdms_collected_opinion_info(index);
                neighborhood_data.byte[i] = (info.kilo_id << 1);
                neighborhood_data.byte[i] = neighborhood_data.byte[i] | (info.opinion == opinion ? 1 : 0);
            }
            waiting_neighborhood_data_sent = 1;
#endif
            break;

        default:
            return;
    }
}


inline static float get_current_sec() {
    return kilo_ticks / (double)TICKS_PER_SEC;
}


inline static float get_duration_sec(kilobot_state_t st){
    switch(st) {
        case DISSEMINATION:
#ifdef DIRECT_COMPARISON
            return g_ran_exponential(1.0/(float)MEAN_DURATION_OF_DISSEMINATION);
#else
            return g_ran_exponential(1.0/(float)(quality * MEAN_DURATION_OF_DISSEMINATION));
#endif
        case EXPLORATION:
            return g_ran_exponential(1.0/(float)MEAN_DURATION_OF_EXPLORATION);
        default:
            return 0;
    }
}


static inline uint8_t disseminate()
{
    IR_message_t *msg = kilob_message_send();
    if(msg == NULL) return 0;

    msg->type = MESSAGE_ADVERTISE_KILOBOT_OPINION;
    msg->data[SECTION_KILOUID]            = kilo_uid;
    msg->data[SECTION_OPINION]            = opinion;
    msg->data[SECTION_DURATION_MATCH]     = duration_match;
    msg->data[SECTION_DURATION_EXPLORATE] = duration_explorate;
    return 1;
}


inline static void send_state_tracking()
{
    // user_byte_data[1] := opinion
    tracking_data.byte[1] = opinion;

    // user_byte_data[2] := state
    //   - 0 = INITIAL
    //   - 1 = EXPLORATION
    //   - 2 = DISSEMINATION
    //   - 3 = DICISION_MAKING
    tracking_data.byte[2] = (uint8_t)state;

    // user_byte_data[3] := action
    tracking_data.byte[3] = (uint8_t)action;

    // user_byte_data[4] := current quality
    tracking_data.byte[4] = (uint8_t)(quality * 0xFF);

#ifdef DEBUG
    // user_byte_data[5] := the number of stored opinions
    tracking_data.byte[5] = (uint8_t)kilob_cdms_count_collected_opinions();
#endif

    kilob_tracking(&tracking_data);
}
