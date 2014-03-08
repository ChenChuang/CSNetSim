#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <cstdlib>

void crunsim(void);
void crunsim_h(void);

int main(int argc, char **argv)
{
	//std::srand((unsigned)time(0));
	std::srand(1);
	
	std::cout << "Hello, CSNetSim\n" << std::endl << std::endl;
	int t0 = std::time(NULL);
	crunsim();
	//crunsim_h();
	int t1 = std::time(NULL);
	std::cout << std::endl << "Elapsed time = " << (t1-t0) << " seconds" << std::endl; 
	std::cout << "Goodbye, CSNetSim\n" << std::endl;
	getchar();
	return 0;
}
