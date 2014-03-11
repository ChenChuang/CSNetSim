#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <cstdlib>

void crunsim(void);
void crunsim_h(void);

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

int main(int argc, char **argv)
{
	std::srand((unsigned)std::time(0));
	//std::srand(1);
	
	std::cout << "Hello, CSNetSim\n" << std::endl << std::endl;
	int t0 = std::time(NULL);
	crunsim();
	//crunsim_h();
	int t1 = std::time(NULL);
	std::cout << std::endl << "Elapsed time = " << (t1-t0) << " seconds" << std::endl; 
	std::cout << "Goodbye, CSNetSim\n" << std::endl;
	std::cout << currentDateTime() << std::endl;
	//getchar();
	return 0;
}
