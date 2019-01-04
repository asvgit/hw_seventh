#include <iostream>

int main(int argc, char *argv[]) {
	try {
		std::cout << "hw_seventh" << std::endl;
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
