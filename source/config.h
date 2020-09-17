# ifndef CONFIG_H
# define CONFIG_H

/* max = 1000
 * min = 0 */
static int speed = 600;

/* max = 1000 
 * min = 0 */
static int spawn_rate = 700;

/* max = 500
 * min = 0 */
static int drip_length = 35;

/* max = 499
 * min = 0 */
static int min_drip_length = 4;

/* colors */
static int base_text_color = GREEN;
static int base_background_color = DEFAULT;
static int head_text_color = WHITE;
static int head_background_color = DEFAULT;

/* bold */
static bool random_bold = false;
static bool all_bold = true;

/* characters */
static bool lower_case_alpha = true;
static bool upper_case_alpha = true;
static bool digits = true;
static bool symbols = true;

# endif
