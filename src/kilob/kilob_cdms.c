#include <stddef.h>

#include "kilob_cdms.h"
#include "ringbuffer.h"

static uint8_t NUMBER_OF_OPINIONS = 0;
static uint8_t GROUP_SIZE         = 0;

RB_create(opinfo_buffer, opinion_info_t, OPINION_BUFFER_CAPACITY);

#define KILOB_CDMS_FOREACH_ITEM(buffer, i) \
    for(unsigned (i) = 0; (i) < kilob_cdms_count_collected_opinions(); ++(i))
  //for(unsigned (i) = RB_begin(buffer); (i) != RB_end(buffer); RB_next(buffer, (i)))


static unsigned opinfo_buffer_index(uint8_t kilouid)
{
  KILOB_CDMS_FOREACH_ITEM(opinfo_buffer, i) {
    if(RB_idx(opinfo_buffer, i).kilo_id == kilouid) return i;
  }
  return kilob_cdms_count_collected_opinions();
}


// static unsigned next(unsigned i) {
//   return RB_next(opinfo_buffer, i);
// }


static void RB_make_newest(unsigned index)
{
  // Copy the target entity
  opinion_info_t target = RB_idx(opinfo_buffer, index);
  // Shift entities between index and the last of ringbuffer
  // for(; next(index) < RB_end(opinfo_buffer); index = next(index)) {
  for( ; index + 1 < kilob_cdms_count_collected_opinions(); ++index) {
    RB_idx(opinfo_buffer, index) = RB_idx(opinfo_buffer, index + 1);
  }
  // Write the target entity to the last
  RB_idx(opinfo_buffer, index) = target;
}


int kilob_cdms_init(uint8_t number_of_opinions, uint8_t group_size)
{
  if(group_size >= OPINION_BUFFER_CAPACITY) return 1;

  // create ring buffer
  RB_init(opinfo_buffer);
  // initialize inner variables
  NUMBER_OF_OPINIONS = number_of_opinions;
  GROUP_SIZE         = group_size;

  return 0;
}


opinion_t kilob_cdms_random_opinion()
{
  // set initial opinion
  const double inc = 1.0/(float)NUMBER_OF_OPINIONS;
  const double rnd = g_ran_uniform();
  opinion_t opinion;
  double    tmp;
  for(opinion = 0, tmp = inc; opinion < NUMBER_OF_OPINIONS - 1; ++opinion) {
    if(rnd > tmp) {
      tmp += inc;
    } else {
      return opinion;
    }
  }
  return opinion;
}


void kilob_cdms_clear_buffer()
{
  RB_clear(opinfo_buffer);
}


void kilob_cdms_collect(uint8_t kilouid, opinion_t opinion, double quality)
{
  int index = opinfo_buffer_index(kilouid);
  opinion_info_t opinfo = {kilouid, opinion, quality};

  if(index < kilob_cdms_count_collected_opinions()) {  // if kilouid is already collected
    // Update collected data
    RB_idx(opinfo_buffer, index) = opinfo;
    // Make current opinfo the newest
    RB_make_newest(index);
  } else {
    // Add new opinion to the buffer
    RB_back(opinfo_buffer) = opinfo;
    RB_pushback(opinfo_buffer);
    // Remove oldest data if size is over GROUP_SIZE
    if(kilob_cdms_count_collected_opinions() > GROUP_SIZE) {
      RB_popfront(opinfo_buffer);
    }
  }
}


unsigned kilob_cdms_count_collected_opinions()
{
  return RB_size(opinfo_buffer);
}


opinion_info_t kilob_cdms_collected_opinion_info(unsigned index)
{
  return RB_idx(opinfo_buffer, index);
}


opinion_t kilob_cdms_majority_rule(opinion_t self)
{
  if(RB_size(opinfo_buffer) < 1) return self;
  uint8_t tie = 0;
  opinion_t ret = self;
  unsigned  count_ret = 0;
  for(opinion_t o = 0; o < NUMBER_OF_OPINIONS; ++o) {
    unsigned count = (o == self ? 1 : 0);
    KILOB_CDMS_FOREACH_ITEM(opinfo_buffer, i) {
      if(RB_idx(opinfo_buffer, i).opinion == o) {
        ++count;
      }
    }
    if(count == count_ret){
    tie = 1;}
    if(count > count_ret) {
      ret = o;
      count_ret = count;
      tie = 0;
    }
  }
  if(tie==1) return self;
  else return ret;
}


opinion_t kilob_cdms_voter_model(opinion_t self)
{
  uint8_t chosen_kilo_uid;

  return kilob_cdms_voter_model_with_kilouid(self, &chosen_kilo_uid);
}


opinion_t kilob_cdms_voter_model_with_kilouid(opinion_t self, uint8_t* chosen_kilouid)
{
  const unsigned size = RB_size(opinfo_buffer);
  double rnd = g_ran_uniform();
  opinion_t ret = self;
  double tmp = 1/(double)size;

  *chosen_kilouid = kilo_uid;

  if(size < 1) return self;

  KILOB_CDMS_FOREACH_ITEM(opinfo_buffer, i) {
    opinion_info_t* check = &(RB_idx(opinfo_buffer, i));
    ret = check->opinion;
    *chosen_kilouid = check->kilo_id;
    if(rnd < tmp) {
      return ret;
    }
    tmp += 1/(double)size;
  }

  return ret;
}


opinion_t kilob_cdms_unanimity_rule(opinion_t self)
{
  opinion_t new_opinion = RB_front(opinfo_buffer).opinion;

  KILOB_CDMS_FOREACH_ITEM(opinfo_buffer, i) {
    opinion_t to_check = RB_idx(opinfo_buffer, i).opinion;
    if(to_check != new_opinion) return self;
  }
  return new_opinion;
}


opinion_t kilob_cdms_direct_comparison(opinion_t self, double quality)
{
  KILOB_CDMS_FOREACH_ITEM(opinfo_buffer, i) {
    double q = RB_idx(opinfo_buffer, i).quality;
    if( q > quality ) {
      self = RB_idx(opinfo_buffer, i).opinion;
      quality = q;
    }
  }
  return self;
}


void kilob_cdms_get_last_neighborhoods(uint8_t kilouids[])
{
  KILOB_CDMS_FOREACH_ITEM(opinfo_buffer, i) {
    *(kilouids++) = RB_idx(opinfo_buffer, i).kilo_id;
  }
}
