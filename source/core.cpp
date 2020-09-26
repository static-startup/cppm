# include <ncurses.h>
# include <locale.h>
# include <iostream>
# include <stdlib.h>
# include <stdio.h>
# include <vector>
# include <chrono>
# include <string>
# include <time.h>
# include <array>
# include "config.h"
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

std::vector<wint_t> matrix_symbols = {
	'-', '=', '*', '_', '+', '|', ':', '<', '>', '"',
};

std::vector<wint_t> japanese = {
	L'ｦ', L'ｧ', L'ｨ', L'ｩ', L'ｪ', L'ｫ', L'ｬ', L'ｭ', L'ｮ', L'ｯ',
	L'ｰ', L'ｱ', L'ｲ', L'ｳ', L'ｴ', L'ｵ', L'ｶ', L'ｷ', L'ｸ', L'ｹ',
	L'ｺ', L'ｻ', L'ｼ', L'ｽ', L'ｾ', L'ｿ', L'ﾀ', L'ﾁ', L'ﾂ', L'ﾃ',
	L'ﾄ', L'ﾅ', L'ﾆ', L'ﾇ', L'ﾈ', L'ﾉ', L'ﾊ', L'ﾋ', L'ﾌ', L'ﾍ',
	L'ﾎ', L'ﾏ', L'ﾐ', L'ﾑ', L'ﾒ', L'ﾓ', L'ﾔ', L'ﾕ', L'ﾖ', L'ﾗ',
	L'ﾘ', L'ﾙ', L'ﾚ', L'ﾛ', L'ﾜ', L'ﾝ',
};

std::vector<wint_t> greek = {
	L'α', L'β', L'γ', L'δ', L'ε', L'ζ', L'η', L'θ', L'ι', L'κ',
	L'λ', L'μ', L'ν', L'ξ', L'ο', L'π', L'ρ', L'σ', L'ς', L'τ',
	L'υ', L'φ', L'χ', L'ψ', L'ω',
};

class user_interface {
	public:

		struct node {
			public:
				
				wint_t value;
				int attribute;
				bool is_head;
				bool flash = false;
		};

		unsigned long timer = current_time();

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
		}

		void init_color_pairs() {
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
			if(!setlocale(LC_ALL, "")) {
				std::cout << "warning: no locale set." << std::endl;
			}
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

		void bound_config() {
			if(delay > 10 || delay < 0) {
				quit("invalid delay " + std::to_string(delay));
			} else if(drip_spawn_rate > 10 || drip_spawn_rate < 0) {
				quit("invalid spawn rate " + std::to_string(delay));
			} else if(drip_length > 50 || drip_length < 0) {
				quit("invalid drip length " + std::to_string(drip_length));
			} else if(min_drip_length > 49 || min_drip_length < 0) {
				quit("invalid min drip length " + std::to_string(drip_length));
			}
		}

		std::vector<std::vector<node>> init_matrix() {
			std::vector<std::vector<node>> matrix;
			for(int i = 0; i < LINES; i++) {
				matrix.push_back(std::vector<node>());
				for(int k = 0; k < COLS; k++) {
					matrix[i].push_back({' ', COLOR_PAIR(matrix_color.size() + 2), false});
				}
			}
			return matrix;
		}

		void draw_matrix(std::vector<std::vector<node>> matrix) {
			for(int i = 0; i < matrix.size(); i++) {
				for(int k = 0; k < matrix[0].size(); k++) {
					wattron(stdscr, matrix[i][k].attribute);
					mvwprintw(stdscr, i, k, "%lc", matrix[i][k].value);
					wattroff(stdscr, matrix[i][k].attribute);
				}
			}
		}

		std::vector<std::vector<node>> update_matrix(std::vector<std::vector<node>> matrix) {
			for(int k = 0; k < matrix[0].size(); k++) {
				for(int i = 0; i < matrix.size(); i++) {
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
							matrix[i][k].flash = false;
						}

						// does it have no tail
						if(i == 0) {
							no_tail = true;
						}
								
						// loop until head is found
						while(i != matrix.size() - 1 && !matrix[i][k].is_head) {
							if(matrix[i][k].flash) {
								matrix[i][k].value = random_character();
							}
							++i;
						}

						// if no tail randomly add tail
						if(no_tail
						&& rand() % drip_length == 0
						&& i > min_drip_length) {

							matrix[0][k].value = ' ';
							matrix[0][k].attribute = COLOR_PAIR(matrix_color.size() + 2);
						}

						if(i != matrix.size() - 1) { // move head
							matrix[i][k].is_head = false;
							matrix[i][k].attribute = random_attribute(false);

							if(i != matrix.size() - 2
							&& rand() % 15 == 0
							&& random_speed) {

								matrix[i + 1][k].is_head = false;
								matrix[i + 1][k].value = random_character();
								matrix[i + 1][k].attribute = random_attribute(false);
								matrix[i + 1][k].flash = flashes && rand() % 6 == 0 ? true : false;

								matrix[i + 2][k].is_head = true;
								matrix[i + 2][k].value = random_character();
								matrix[i + 2][k].attribute = random_attribute(true);
								matrix[i + 2][k].flash = flashes && rand() % 6 == 0 ? true : false;
							} else {
								matrix[i + 1][k].is_head = true;
								matrix[i + 1][k].value = random_character();
								matrix[i + 1][k].attribute = random_attribute(true);
								matrix[i + 1][k].flash = flashes && rand() % 6 == 0 ? true : false;
							}
						} else if(matrix[i][k].is_head) { // remove head
							matrix[i][k].is_head = false;
							matrix[i][k].attribute = random_attribute(false);
						}

						i++;
					}
				}
			}
			return matrix;
		}

		void adjust_config() {
			drip_spawn_rate *= 1000;

			matrix_color = matrix_color.empty() ? std::vector<int>{ 2 } : matrix_color;

			if(characters.empty()) {
				characters.insert(characters.end(), lower_case_alpha.begin(), lower_case_alpha.end());
			}
		}
		
		void loop() {
			bound_config();
			adjust_config();
			init_color_pairs();
			
			std::vector<std::vector<node>> matrix = init_matrix();

			while(getch() == ERR) {
				if(timer < current_time()) {
					refresh();
					draw_matrix(matrix);
					matrix = update_matrix(matrix);
					timer = current_time() + delay * 8;
				}
			}

			endwin();
			exit(0);
		}
};

void print_usage() {
	std::cout << " Usage: cppm [-d DELAY] [-sr SPAWN_RATE] [-l MAX_LENGTH] [-m MIN_LENGTH] [-b] [-B]"
			  << "[-r] [-R] [-h] [-hc COLOR] [-bc COLOR] [-s] [-c COLOR] [-cs CHARACTER_SETS] [-f]\n\n";
	std::cout << " -d:  delay (0 - 10) delay before screen update\n";
	std::cout << " -sr: spawn rate (0 - 10) of rows\n";
	std::cout << " -l:  length (0 - 50) row length\n";
	std::cout << " -m:  minimal length (0 - 49) minimal row length\n";
	std::cout << " -b:  random bold characters\n";
	std::cout << " -B:  every character is bold\n";
	std::cout << " -r:  every character's color is rainbow\n";
	std::cout << " -R:  every character's background color and foreground color is reversed\n";
	std::cout << " -h:  print usage\n";
	std::cout << " -hc: head color\n";
	std::cout << " -bc: background color\n";
	std::cout << " -s:  random speed\n";
	std::cout << " -c:  set matrix color\n";
	std::cout << " -f:  character flashes to different character\n";
	std::cout << " -cs: set character sets\n\n";
	std::cout << " Colors: black, red, yellow, green, blue, magenta, cyan, white, none\n\n";
	std::cout << " Character Sets:\n a: lower case alphabet\n A: upper case alphabet\n d: 0-9 numbers"
			  << "\n s: common symbols\n j: half-width japanese kana\n g: greek symbols\n S: symbols from matrix movie\n";
	std::cout << "\n Examples:\n cppm -cs SSsggg \n cppm -cs SSgjjj\n cppm -r -R -b\n";
	exit(0);
}

int process_color(std::string color) {
	int color_code;
	if(color == "none") {
		color_code = -1;
	} else if(color == "black") {
		color_code = 0;
	} else if(color == "red") {
		color_code = 1;
	} else if(color == "green") {
		color_code = 2;
	} else if(color == "yellow") {
		color_code = 3;
	} else if(color == "blue") {
		color_code = 4;
	} else if(color == "magenta") {
		color_code = 5;
	} else if(color == "cyan") {
		color_code = 6;
	} else if(color == "white") {
		color_code = 7;
	} else {
		std::cout << "color " << color << " is invalid.\n";
		std::cout << " Colors: black, red, yellow, green, blue, magenta, cyan, white, none\n";
		exit(0);
	}
	return color_code;
}

void process_arguments(int argc, char *argv[]) {
	for(int i = 1; i < argc; i++) {
		if(std::string(argv[i]) == "-d" && i != argc - 1) {
			delay = std::stoi(argv[i + 1]);
			i++;
		}
		
		else if(std::string(argv[i]) == "-sr" && i != argc - 1) {
			drip_spawn_rate = std::stoi(argv[i + 1]);
			i++;
		}
		
		else if(std::string(argv[i]) == "-l" && i != argc - 1) {
			drip_length = std::stoi(argv[i + 1]);
			i++;
		}
		
		else if(std::string(argv[i]) == "-m" && i != argc - 1) {
			min_drip_length = std::stoi(argv[i + 1]);
			i++;
		}
		
		else if(std::string(argv[i]) == "-b") {
			random_bold = true;
		}
		
		else if(std::string(argv[i]) == "-B") {
			all_bold = true;
		}
		
		else if(std::string(argv[i]) == "-r") {
			rainbow = true;
		}
		
		else if(std::string(argv[i]) == "-R") {
			reverse = true;
		}
		
		else if(std::string(argv[i]) == "-h") {
			print_usage();
			exit(0);
		}

		else if(std::string(argv[i]) == "-hc" && i != argc - 1) {
			head_color = process_color(argv[i + 1]);
			i++;
		}

		else if(std::string(argv[i]) == "-bc" && i != argc - 1) {
			background_color = process_color(argv[i + 1]);
			i++;
		}

		else if(std::string(argv[i]) == "-c") {
			i++;
			while(i != argc) {
				if(argv[i][0] == '-') {
					--i;
					break;
				}
				matrix_color.push_back(process_color(argv[i]));
				++i;
			}
		}

		else if(std::string(argv[i]) == "-s") {
			random_speed = true;
		}

		else if(std::string(argv[i]) == "-f") {
			flashes = true;
		}

		else if(std::string(argv[i]) == "-cs" && i != argc - 1) {
			for(int k = 0; k < std::string(argv[i + 1]).length(); k++) {
				if(argv[i + 1][k] == 'a') {
					characters.insert(characters.end(), lower_case_alpha.begin(), lower_case_alpha.end());
				} else if(argv[i + 1][k] == 'A') {
					characters.insert(characters.end(), upper_case_alpha.begin(), upper_case_alpha.end());
				} else if(argv[i + 1][k] == 'd') {
					characters.insert(characters.end(), digits.begin(), digits.end());
				} else if(argv[i + 1][k] == 's') {
					characters.insert(characters.end(), symbols.begin(), symbols.end());
				} else if(argv[i + 1][k] == 'j') {
					characters.insert(characters.end(), japanese.begin(), japanese.end());
				} else if(argv[i + 1][k] == 'g') {
					characters.insert(characters.end(), greek.begin(), greek.end());
				} else if(argv[i + 1][k] == 'S') {
					characters.insert(characters.end(), matrix_symbols.begin(), matrix_symbols.end());
				} else {
					std::cout << "Invalid Characted Set \"" << argv[i + 1][k] << "\"\n";
					exit(0);
				}
			}
			i++;
		}

		else {
			print_usage();
			exit(1);
		}
	}
}

int main(int argc, char *argv[]) {
	process_arguments(argc, argv);

	srand(time(NULL));

	user_interface ui;
	ui.init_ncurses();
	ui.loop();
}
