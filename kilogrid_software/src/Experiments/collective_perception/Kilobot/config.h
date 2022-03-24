#ifndef CONFIG_H
#define CONFIG_H

#define NUMBER_OF_OPINIONS              5
#define GROUP_SIZE                      5

#define MEAN_DURATION_OF_DISSEMINATION  (15 * 60.0)
#define MEAN_DURATION_OF_EXPLORATION    (5 * 60.0)

#define RANDOM_WALK_FORWARD_MEAN        15.0
#define RANDOM_WALK_FORWARD_SIGMA       1.5
#define RANDOM_WALK_ROT_CONST           0.5
#define RANDOM_WALK_ROT_SPAN            3.0

#define BLINK_SPAN                      500
#define BLINK_COLOR                     RGB(0,0,0)

#define DISSEMINATION_INTERVAL          0.50  // [sec]


#endif  /* CONFIG_H */
