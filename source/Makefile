CC = g++
LIBS = -lncurses

install: core.cpp
	${CC} core.cpp ${LIBS} -o cppm
	sudo mv ./cppm /usr/bin

uninstall:
	sudo rm /usr/bin/cppm > /dev/null 2>&1 || echo -e "\e[31mcppm not installed\e[0m"; exit 0
