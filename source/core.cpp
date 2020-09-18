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

		void include_charsets() {
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
						}

						// does it have no tail
						if(i == 0) {
							no_tail = true;
						}
								
						// loop until head is found
						while(i != matrix.size() - 1 && !matrix[i][k].is_head) {
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
			include_charsets();
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
	std::cout << " Usage: cppm -[hbBRaAdSj] [-d delay] [-sr spawn rate]"
			  << "[-l max length] [-m min length] [-H head color] [-C background color] [-c text color]\n";
	std::cout << " -h: print usage\n";
	std::cout << " -b: half bold characters\n";
	std::cout << " -B: all bold characters\n";
	std::cout << " -r: rainbow mode\n";
	std::cout << " -R: reverse mode\n";
	std::cout << " -a: include lower case alphabet\n";
	std::cout << " -A: include upper case alphabet\n";
	std::cout << " -d: include 0-9 digits\n";
	std::cout << " -S: include symbols\n";
	std::cout << " -j: include half-width japanese kata\n";
	std::cout << " -d: delay (0 - 10) before updating screen\n";
	std::cout << " -sr: spawn rate (0 - 10) row spawn rate\n";
	std::cout << " -l: max length (0 - 50) row max length\n";
	std::cout << " -m: min length (0 - 49) row min length\n";
	std::cout << " -hc: set head color\n";
	std::cout << " -bc: set background color\n";
	std::cout << " -c: set character colors\n";
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
		
		else if(std::string(argv[i]) == "-a") {
			include_lower_case_alpha = true;
		}
		
		else if(std::string(argv[i]) == "-A") {
			include_upper_case_alpha = true;
		}
		 
		else if(std::string(argv[i]) == "-d") {
			include_digits = true;
		}
		
		else if(std::string(argv[i]) == "-S") {
			include_symbols = true;
		}
		
		else if(std::string(argv[i]) == "-j") {
			include_japanese = true;
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
