# ifndef CONFIG_H
# define CONFIG_H

static int delay = 5;
static int drip_spawn_rate = 5;
static int drip_length = 25;
static int min_drip_length = 5;

/* colors */
static std::vector<int> matrix_color;

static int head_color = 7;
static int background_color = -1;

/* bold */
static bool random_bold = false;
static bool all_bold = false;

/* characters */
static bool include_lower_case_alpha = false;
static bool include_upper_case_alpha = false;
static bool include_digits = false;
static bool include_symbols = false;
static bool include_japanese = false;

/* random */
static bool rainbow = false;
static bool reverse = false;

# endif
