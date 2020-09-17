# ifndef CONFIG_H
# define CONFIG_H

/* max = 100
 * min = 0 */
static int speed = 60;

/* max = 100 
 * min = 0 */
static int drip_spawn_rate = 60;

/* max = 50
 * min = 0 */
static int drip_length = 35;

/* max = 49
 * min = 0 */
static int min_drip_length = 4;

/* colors */
static std::vector<int> matrix_color = { GREEN };

static int head_color = WHITE;
static int background_color = DEFAULT;

/* bold */
static bool random_bold = true;
static bool all_bold = false;

/* characters */
static bool include_lower_case_alpha = false;
static bool include_upper_case_alpha = false;
static bool include_digits = false;
static bool include_symbols = false;
static bool include_japanese = true;

/* random */
static bool rainbow = false;
static bool reverse = false;

# endif
