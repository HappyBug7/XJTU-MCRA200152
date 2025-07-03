#include <windows.h>
#include <iostream>
#include <conio.h>
#include "winio.h"

#pragma comment(lib,"winio.lib")
using namespace std;

/**
* inputs:
* 0 -> 1 call
* 1 -> 2 call
* 2 -> 1 sensor
* 3 -> 2 sensor
* 4 -> door close
* 5 -> door open
* outputs:
* 0 -> lift at 1
* 1 -> lift at 2
* 2 -> door open
* 3 -> door closed
* 4 -> lift going up
* 5 -> lift going down
*/

unsigned int BASE_ADDRESS = 0x3f40;
int iPort = 16;
int oPort = 16;

void close_door(unsigned& current_state) {
	bool is_open = (current_state >> 2) & 0x01;
	current_state = is_open ? (current_state | 0x08) & (~0x04) : current_state;
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
}

void open_door(unsigned& current_state) {
	bool is_open = (current_state >> 2) & 0x01;
	current_state = !is_open ? (current_state | 0x04) & (~0x08) : current_state;
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
}

void move_lift(unsigned& current_state) {
	bool is_first_floor = current_state & 0x01;
	int input;
	bool is_arrived;
	if (is_first_floor) {
		// going up
		current_state = current_state | 0x010;
		SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
		while (true) {
			DWORD* p = new DWORD;
			GetPortVal(BASE_ADDRESS + iPort, p, 1);
			input = *p;
			is_arrived = (~input >> 3) & 0x01;
			if (is_arrived) {
				current_state = current_state & (~0x01);
				current_state = current_state | 0x02;
				current_state = current_state & (~0x10);
				SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
				break;
			}
		}
	}
	else {
		// going down
		current_state = current_state | 0x20;
		SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
		while (true) {
			DWORD* p = new DWORD;
			GetPortVal(BASE_ADDRESS + iPort, p, 1);
			input = *p;
			is_arrived = (~input >> 2) & 0x01;
			if (is_arrived) {
				current_state = current_state & (~0x02);
				current_state = current_state | 0x01;
				current_state = current_state & (~0x20);
				SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
				break;
			}
		}
	}
}

void call_lift(unsigned& current_state, bool from_first_floor) {
	// different floor
	bool at_first = current_state & 0x01;
	if (at_first & !from_first_floor || !at_first & from_first_floor) {
		move_lift(current_state);
	}
	open_door(current_state);
	// 500 * 10ms = 5000ms = 5s
	int cnt = 500;
	int input;
	while (cnt > 0) {
		DWORD* p = new DWORD;
		GetPortVal(BASE_ADDRESS + iPort, p, 1);
		input = *p;
		if ((~input >> 4) & 0x01) {
			break;
		}
		Sleep(10);
		cnt--;
	}
	close_door(current_state);
	move_lift(current_state);
	open_door(current_state);
	cnt = 500;
	while (cnt > 0) {
		DWORD* p = new DWORD;
		GetPortVal(BASE_ADDRESS + iPort, p, 1);
		input = *p;
		if ((~input >> 4) & 0x01) {
			break;
		}
		Sleep(10);
		cnt--;
	}
	close_door(current_state);
}

int main() {
	DWORD* p = new DWORD;
	int input = 0;
	int output = 0;
	unsigned current_state = 0xA;
	if (!InitializeWinIo()) {
		std::cout << "Error in Initialization" << std::endl;
	}
	SetPortVal(BASE_ADDRESS + oPort, current_state, 1);
	while (true) {
		GetPortVal(BASE_ADDRESS + iPort, p, 1);
		input = ~(*p);
		if (input & 0x01 || (input >> 1) & 0x01) {
			call_lift(current_state, input & 0x01);
		}
		Sleep(10);
	}
	ShutdownWinIo();
	return 0;
}
