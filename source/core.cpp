# include <locale.h>
# include <ncurses.h>
# include <iostream>
# include <stdlib.h>
# include <stdio.h>
# include <vector>
# include <chrono>
# include <string>
# include <time.h>
# include <array>

static constexpr int BLACK =   0;
static constexpr int RED =     1;
static constexpr int GREEN =   2;
static constexpr int YELLOW =  3;
static constexpr int BLUE =    4;
static constexpr int MAGENTA = 5;
static constexpr int CYAN =    6;
static constexpr int WHITE =   7;

static constexpr int DEFAULT = -1;
static constexpr int BOLD = A_BOLD;

# include "config.h"

class user_interface {
	public:

		struct node {
			public:
				
				wint_t value;
				int attribute;
				bool is_head;
		};

		unsigned long timer = current_time();

		std::vector<wint_t> characters;

		std::vector<wint_t> lower_case_alpha = {
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
			'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
			'u', 'v', 'w', 'x', 'y', 'z',
		};

		std::vector<wint_t> upper_case_alpha = {
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
			'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		};


		std::vector<wint_t> digits = {
			'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
		};

		std::vector<wint_t> symbols = {
			'!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
			'`', '~', '<', '>', '?', '/', '\\', '"', '_', '-',
			';', ':', '\'', ',', '.', '|', '{', '}', '[', ']',
		};

		std::vector<wint_t> japanese = {
			L'ｦ', L'ｧ', L'ｨ', L'ｩ', L'ｪ', L'ｫ', L'ｬ', L'ｭ', L'ｮ', L'ｯ',
			L'ｰ', L'ｱ', L'ｲ', L'ｳ', L'ｴ', L'ｵ', L'ｶ', L'ｷ', L'ｸ', L'ｹ',
			L'ｺ', L'ｻ', L'ｼ', L'ｽ', L'ｾ', L'ｿ', L'ﾀ', L'ﾁ', L'ﾂ', L'ﾃ',
			L'ﾄ', L'ﾅ', L'ﾆ', L'ﾇ', L'ﾈ', L'ﾉ', L'ﾊ', L'ﾋ', L'ﾌ', L'ﾍ',
			L'ﾎ', L'ﾏ', L'ﾐ', L'ﾑ', L'ﾒ', L'ﾓ', L'ﾔ', L'ﾕ', L'ﾖ', L'ﾗ',
			L'ﾘ', L'ﾙ', L'ﾚ', L'ﾛ', L'ﾜ', L'ﾝ',
		};

		int matrix_w, matrix_h;

		unsigned long current_time() {
			return std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now().time_since_epoch()).count();
		}

		void init_colors() {
			if(!has_colors()) {
				endwin();
				std::cout << "your terminal does not support color" << std::endl;
				exit(0);
			}
			start_color();
			use_default_colors();

			if(rainbow) {
				matrix_color = {1, 2, 3, 4, 5, 6, 7};
			}

			for(int i = 0; i < matrix_color.size(); i++) {
				init_pair(i + 1, matrix_color[i], background_color);
			}

			init_pair(matrix_color.size() + 2, 0, background_color);
			init_pair(matrix_color.size() + 1, head_color, background_color);
		}

		void init_ncurses() {
			setlocale(LC_ALL, "");
			initscr();
			cbreak();
			noecho();
			nodelay(stdscr, true);
			keypad(stdscr, true);
			init_colors();
			curs_set(false);
		}

		wint_t random_character() {
			return characters[rand() % characters.size()];
		}

		int random_attribute(bool is_head) {
			int attributes = 0;
			attributes = attributes|(reverse ? A_REVERSE : 0);
			attributes = attributes|(all_bold ? A_BOLD : 0);
			attributes = attributes|(random_bold && !all_bold && rand() % 2 == 0 ? A_BOLD : 0);

			if(!is_head) {
				attributes = attributes|(COLOR_PAIR(rand() % matrix_color.size() + 1));
			} else {
				attributes = attributes|(COLOR_PAIR(1 + matrix_color.size()));
			}

			return attributes;
		}

		void quit(std::string message) {
			endwin();
			std::cout << message << std::endl;
			exit(0);
		}

		void loop() {
			if(speed > 100 || speed < 0) {
				quit("invalid speed " + std::to_string(speed));
			} else if(drip_spawn_rate > 100 || drip_spawn_rate < 0) {
				quit("invalid spawn rate " + std::to_string(speed));
			} else if(drip_length > 50 || drip_length < 0) {
				quit("invalid drip length " + std::to_string(drip_length));
			} else if(min_drip_length > 49 || min_drip_length < 0) {
				quit("invalid min drip length " + std::to_string(drip_length));
			}

			if(include_lower_case_alpha) {
				characters.insert(characters.end(), lower_case_alpha.begin(), lower_case_alpha.end());
			} if(include_upper_case_alpha) {
				characters.insert(characters.end(), upper_case_alpha.begin(), upper_case_alpha.end());
			} if(include_digits) {
				characters.insert(characters.end(), digits.begin(), digits.end());
			} if(include_symbols) {
				characters.insert(characters.end(), symbols.begin(), symbols.end());
			} if(include_japanese) {
				characters.insert(characters.end(), japanese.begin(), japanese.end());
			}

			speed = 100 - speed;
			drip_spawn_rate *= 100;

			matrix_w = COLS;
			matrix_h = LINES;

			if(characters.empty()) {
				quit("no characters");
			}

			node matrix[matrix_h][matrix_w];

			for(int i = 0; i < matrix_h; i++) {
				for(int k = 0; k < matrix_w; k++) {
					if(i == 0) {
						if(rand() % drip_spawn_rate / 150 == 0) {
							matrix[i][k] = {random_character(), random_attribute(true), true};
						} else {
							matrix[i][k] = {' ', COLOR_PAIR(matrix_color.size() + 2), false};
						}
					} else {
						matrix[i][k] = {' ', COLOR_PAIR(matrix_color.size() + 2), false};
					}
				}
			}

			while(getch() == ERR) {
				if(timer < current_time()) {
					refresh();

					for(int i = 0; i < matrix_h; i++) {
						for(int k = 0; k < matrix_w; k++) {
							wattron(stdscr, matrix[i][k].attribute);
							mvwprintw(stdscr, i, k, "%lc", matrix[i][k].value);
							wattroff(stdscr, COLOR_PAIR(1)
											|COLOR_PAIR(2)
											|A_BOLD
											|A_REVERSE);

							if(rainbow) {
								wattroff(stdscr, COLOR_PAIR(3)
												|COLOR_PAIR(4)
												|COLOR_PAIR(5)
												|COLOR_PAIR(6)
												|COLOR_PAIR(7)
												|COLOR_PAIR(8)
												|COLOR_PAIR(9));
							}
						}
					}

					for(int k = 0; k < matrix_w; k++) {
						for(int i = 0; i < matrix_h; i++) {
							// add random code drip
							if(rand() % drip_spawn_rate == 0
							&& matrix[0][k].value == ' ') {

								matrix[0][k].is_head = true;
								matrix[0][k].value = random_character();
								matrix[0][k].attribute = random_attribute(true);
							}

							// if tail of column found
							if(matrix[i][k].value != ' ') {
								bool no_tail = false;

								// remove tail
								if(i != 0) {
									matrix[i][k].value = ' ';
									matrix[i][k].attribute = COLOR_PAIR(matrix_color.size() + 2);
								}

								// does it have no tail
								if(i == 0) {
									no_tail = true;
								}
								
								// loop until head is found
								while(i != matrix_h - 1 && !matrix[i][k].is_head) {
									++i;
								}

								// if no tail randomly add tail
								if(no_tail
								&& rand() % drip_length == 0
								&& i > min_drip_length) {

									matrix[0][k].value = ' ';
									matrix[0][k].attribute = COLOR_PAIR(matrix_color.size() + 2);
								}

								if(i != matrix_h - 1) { // move head
									matrix[i][k].is_head = false;
									matrix[i][k].attribute = random_attribute(false);

									matrix[i + 1][k].is_head = true;
									matrix[i + 1][k].value = random_character();
									matrix[i + 1][k].attribute = random_attribute(true);
								} else if(matrix[i][k].is_head) { // remove head
									matrix[i][k].is_head = false;
									matrix[i][k].attribute = random_attribute(false);
								}

								i++;
							}
						}
					}

					timer = current_time() + speed;
				}
			}

			endwin();
			exit(0);
		}
};

int main() {
	srand(time(NULL));

	user_interface ui;
	ui.init_ncurses();
	ui.loop();
}
