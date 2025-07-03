#include <windows.h>
#include <iostream>
#include <conio.h>
#include "winio.h"

#pragma comment(lib,"winio.lib")
using namespace std;

/**
* initial:
* M3 -> M2 -> M1
*
*
* stop:
* M1 -> M2 -> M3
*
* output:
* 0 -> M1
* 1 -> M2
* 2 -> M3
*
* 3 -> green
* 4 -> yellow
* 5 -> red
*
* input:
* 0 -> start
* 1 -> end
* 2 - 5 -> load (0-15)
*/

unsigned int BASE_ADDRESS = 0x3f40;
int iPort = 16;
int oPort = 16;

void initial(unsigned& current_state) {
	std::cout << "Initializing!" << std::endl;
	current_state = 0x04;
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
	Sleep(300);
	current_state = 0x06;
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
	Sleep(300);
	current_state = 0x07;
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
	Sleep(300);
	current_state = 0x0f;
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
}

void stop(unsigned& current_state) {
	std::cout << "Stopping!" << std::endl;
	current_state = 0x07;
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
	Sleep(300);
	current_state = 0x06;
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
	Sleep(300);
	current_state = 0x04;
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
	current_state = 0x00;
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
}

void load_check(unsigned& current_state) {
	DWORD* p = new DWORD;
	int input;
	GetPortVal(BASE_ADDRESS + iPort, p, 1);
	input = ~(*p);
	int load = (input >> 2) & 0x0f;
	if (load <= 10) {
		current_state = current_state & (~0x38);
		current_state = current_state | 0x08;
	}
	else if (load <= 13) {
		current_state = current_state & (~0x38);
		current_state = current_state | 0x10;
	}
	else {
		current_state = current_state & (~0x38);
		current_state = current_state | 0x20;
	}
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
}

int main() {
	DWORD* p = new DWORD;
	int input = 0;
	int output = 0;
	unsigned current_state = 0;
	int cnt = 300;
	bool is_error = false;

	if (!InitializeWinIo()) {
		std::cout << "Error in Initialization" << std::endl;
	}
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
	while (true) {
		GetPortVal(BASE_ADDRESS + iPort, p, 1);
		input = ~(*p);
		if ((input >> 7) & 0x01 | (input >> 1) & 0x01) {
			stop(current_state);
		}
		else if ((input & 0x01) && !(current_state & (0x38))) {
			initial(current_state);
		}
		else {
			if (!(current_state & (0x38))) {
				continue;
			}
			load_check(current_state);
			if ((current_state >> 5) & 0x01) {
				if (is_error) {
					if (!cnt) {
						stop(current_state);
						is_error = false;
					}
					else {
						cnt--;
					}
				}
				else {
					std::cout << "overload!" << std::endl;
					is_error = true;
					cnt = 300;
				}
			}
			else {
				is_error = false;
			}
		}
		Sleep(10);
	}
	ShutdownWinIo();
	return 0;
}