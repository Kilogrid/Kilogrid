#ifndef KILOB_CDMS_H
#define KILOB_CDMS_H

#include <stdint.h>
#include "message.h"
#include "kilolib.h"
#include "kilo_rand_lib.h"

/**
 * @typedef opinion_t
 * @brief Opinion type
 */
typedef uint8_t opinion_t;


/**
 * @typedef opinion_info_t
 * @brief opinion information type
 */
typedef struct {
  uint8_t   kilo_id;  // used to distinguish neighborhood kilobots
  opinion_t opinion;
  double    quality;  // used only in direct comparison
} opinion_info_t;


// Let default opinion buffer capacity be 32
// 2^n is efficient number for ringbuffer
#if !defined(OPINION_BUFFER_CAPACITY)

# define OPINION_BUFFER_CAPACITY 32

#endif


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief initialize CMDS
 * @param number_of_opinions number of opinions
 * @param group_size group size for collecting opinions
 * @return 0 if it is successfully initialized
 */
int kilob_cdms_init(uint8_t number_of_opinions, uint8_t group_size);

/**
 */
opinion_t kilob_cdms_random_opinion();

/**
 * @brief clear internal ring buffer
 */
void kilob_cdms_clear_buffer();

/**
 * @brief push back given opinion information to internal ring buffer
 */
void kilob_cdms_collect(uint8_t kilouid, opinion_t opinion, double quality);

/**
 * @brief the number of collected neighborhood opinions
 */
unsigned kilob_cdms_count_collected_opinions();

/**
 * @brief it returns a collected opinion info.
 * @param index should be from 0 to the number of collected opinions
 * @see kilob_cdms_count_collected_opinions
 */
opinion_info_t kilob_cdms_collected_opinion_info(unsigned index);

/**
 */
static inline double kilob_cdms_duration(double mean)
{
  return g_ran_exponential(1.0/mean);
}

/**
 * @brief Majority rule
 * @param self : own opinion
 * @return new opinion if neighborhood opinions are available; otherwise, self opinion
 */
opinion_t kilob_cdms_majority_rule(opinion_t self);

/**
 * @brief Voter model
 * @param self : own opinion
 * @return new opinion if neighborhood opinions are available; otherwise, self opinion
 */
opinion_t kilob_cdms_voter_model(opinion_t self);

/**
 * @brief Voter model with choosen kilouid
 * @param self : own opinion
 * @param uid : pointer to choosen kilouid
 * @return new opinion if neighborhood opinions are available; otherwise, self opinion
 * @note It returns choosen kilouid via pointer
 */
opinion_t kilob_cdms_voter_model_with_kilouid(opinion_t self, uint8_t* choosen_kilouid);

/**
 * @brief k-Unanimity rule
 * @param self : own opinion
 * @return new opinion if all of neighborhoods have the same opinion; otherwise, self opinion
 */
opinion_t kilob_cdms_unanimity_rule(opinion_t self);

/**
 * @brief Direct comparison
 * @param self : own opinion
 * @param quality : own quality estimated
 * @return new opinion if neighborhood opinions are available; otherwise, self opinion
 */
opinion_t kilob_cdms_direct_comparison(opinion_t self, double quality);

/**
 * @brief Fetch kilobot uids from the opinion buffer
 * @param[in,out] kilouids : container for returned kilobot uids
 * @note The length of `kilouids` should be equal to group size
 */
void kilob_cdms_get_last_neighborhoods(uint8_t kilouids[]);


#ifdef __cplusplus
}
#endif

#endif  /* KILOB_CDMS_H */
