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
				
				char value;
				bool is_head;
		};

		unsigned long timer = current_time();

		std::vector<char> characters;

		std::vector<char> lower_case_letters = {
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
			'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
			'u', 'v', 'w', 'x', 'y', 'z',
		};

		std::vector<char> upper_case_letters = {
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
			'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
			'U', 'V', 'W', 'X', 'Y', 'Z',
		};


		std::vector<char> numbers = {
			'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
		};

		std::vector<char> special = {
			'!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
			'`', '~', '<', '>', '?', '/', '\\', '"', '_', '-',
			';', ':', '\'', ',', '.', '|', '{', '}', '[', ']',
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

			init_pair(1, base_text_color, base_background_color);
			init_pair(2, head_text_color, head_background_color);
		}

		void init_ncurses() {
			initscr();
			cbreak();
			noecho();
			nodelay(stdscr, true);
			keypad(stdscr, true);
			init_colors();
			curs_set(false);
		}

		char random_character() {
			return characters[rand() % characters.size() - 1];
		}

		void quit(std::string message) {
			endwin();
			std::cout << message << std::endl;
			exit(0);
		}

		void loop() {
			if(speed > 1000 || speed < 0) {
				quit("invalid speed " + std::to_string(speed));
			}

			if(spawn_rate > 1000 || spawn_rate < 0) {
				quit("invalid spawn rate " + std::to_string(speed));
			}

			if(drip_length > 500 || drip_length < 0) {
				quit("invalid drip length " + std::to_string(drip_length));
			}

			if(min_drip_length > 499 || min_drip_length < 0) {
				quit("invalid min drip length " + std::to_string(drip_length));
			}

			if(lower_case_alpha) {
				characters.insert(characters.end(), lower_case_letters.begin(), lower_case_letters.end());
			}

			if(upper_case_alpha) {
				characters.insert(characters.end(), upper_case_letters.begin(), upper_case_letters.end());
			}

			if(digits) {
				characters.insert(characters.end(), numbers.begin(), numbers.end());
			}

			if(symbols) {
				characters.insert(characters.end(), special.begin(), special.end());
			}

			speed = (1000 - speed) / 10;
			spawn_rate *= 10;

			matrix_w = COLS;
			matrix_h = LINES;

			if(characters.empty()) {
				quit("no characters");
			}

			node matrix[matrix_h][matrix_w];
			int colors[matrix_h][matrix_w];

			for(int i = 0; i < matrix_h; i++) {
				for(int k = 0; k < matrix_w; k++) {
					colors[i][k] = all_bold ? A_BOLD : 0;
					colors[i][k] = rand() % 2 == 0 && random_bold ? A_BOLD : 0;
				}
			}

			for(int i = 0; i < matrix_h; i++) {
				for(int k = 0; k < matrix_w; k++) {
					if(i == 0) {
						if(rand() % spawn_rate / 150 == 0) {
							matrix[i][k] = {random_character(), true};
						} else {
							matrix[i][k] = {' ', false};
						}
					} else {
						matrix[i][k] = {' ', false};
					}
				}
			}

			while(getch() == ERR) {
				if(timer < current_time()) {
					refresh();

					for(int i = 0; i < matrix_h; i++) {
						for(int k = 0; k < matrix_w; k++) {
							wattron(stdscr, colors[i][k]);

							if(matrix[i][k].is_head) {
								wattron(stdscr, COLOR_PAIR(2));
							} else {
								wattron(stdscr, COLOR_PAIR(1));
							}

							mvwaddch(stdscr, i, k, matrix[i][k].value);
							wattroff(stdscr, COLOR_PAIR(1)|COLOR_PAIR(2)|A_BOLD);
						}
					}

					for(int k = 0; k < matrix_w; k++) {
						for(int i = 0; i < matrix_h; i++) {
							// add random code drip
							if(rand() % spawn_rate == 0
							&& matrix[0][k].value == ' ') {

								matrix[0][k].is_head = true;
								matrix[0][k].value = random_character();
							}

							// if tail of column found
							if(matrix[i][k].value != ' ') {
								bool no_tail = false;

								// remove tail
								if(i != 0) {
									matrix[i][k].value = ' ';
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
								}

								if(i != matrix_h - 1) { // move head
									matrix[i][k].is_head = false;
									matrix[i + 1][k].is_head = true;
									matrix[i + 1][k].value = random_character();
								} else if(matrix[i][k].is_head) { // remove head
									matrix[i][k].is_head = false;
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
