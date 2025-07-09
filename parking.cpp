#include <windows.h>
#include <iostream>

#include "winio.h"
#include <conio.h>

#pragma comment(lib,"winio.lib")

int decimal_to_8421(int decimal) {
	int lower_digit = decimal % 10;
	int higher_digit = (decimal / 10) % 10;
	return (higher_digit << 4) + lower_digit;
}

int main() {
	unsigned int BASE_ADDRESS = 0x3f40;
	int iPort = 16;
	int oPort = 16;

	DWORD *p = new DWORD;
	int input = 0;
	int input_prev = 0;
	int output = 0;
	bool is_in = 0;
	bool is_out = 0;
	int empty_lot = 50;

	if (!InitializeWinIo()) {
		std::cout << "Error in Initialization" << std::endl;
	}
	std::cout << "停车管理系统，启动！" << std::endl
		<< "M1 遮光下降沿表示停车，M2 遮光下降沿表示取车。" << std::endl;
	while (true) {
		GetPortVal(BASE_ADDRESS + iPort, p, 1);
		input = *p;
		input = ~input;
		

		if (is_in) std::cout << "进车,空余数量减一！" << std::endl;
		if (is_out) std::cout << "出车,空余数量加一！" << std::endl;
		is_out = (((input >> 1) & 0x01) - ((input_prev >> 1) & 0x01)) == 1;
	        is_in = ((input & 0x01) - (input_prev & 0x01)) == 1;
		input_prev = input;
		empty_lot += 1 * is_out - 1 * is_in;
		if (empty_lot >= 100) empty_lot = 99;
		output = decimal_to_8421(empty_lot);
		SetPortVal(BASE_ADDRESS + oPort, output, 1);
		Sleep(10);
		if (_kbhit()) {
			char flag = _getch();
			if (flag == 'q' || flag == 'Q') {
				std::cout << "用户输入退出，退出程序！" << std::endl;
				Sleep(500);
				break;
			}
		}
	}

	ShutdownWinIo();
	return 0;
}
