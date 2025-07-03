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
	std::cout << "ͣ������ϵͳ��������" << std::endl
		<< "M1 �ڹ��½��ر�ʾͣ����M2 �ڹ��½��ر�ʾȡ����" << std::endl;
	while (true) {
		GetPortVal(BASE_ADDRESS + iPort, p, 1);
		input = *p;
		input = ~input;
		

		if (is_in) std::cout << "����,����������һ��" << std::endl;
		if (is_out) std::cout << "����,����������һ��" << std::endl;
		is_out = (((input >> 1) & 0x01) - ((input_prev >> 1) & 0x01)) == 1;
		input_prev = input;
		empty_lot += 1 * is_out - 1 * is_in;
		if (empty_lot >= 100) empty_lot = 99;
		output = decimal_to_8421(empty_lot);
		SetPortVal(BASE_ADDRESS + oPort, output, 1);
		Sleep(10);
		if (_kbhit()) {
			char flag = _getch();
			if (flag == 'q' || flag == 'Q') {
				std::cout << "�û������˳����˳�����" << std::endl;
				Sleep(500);
				break;
			}
		}
	}

	ShutdownWinIo();
	return 0;
}
