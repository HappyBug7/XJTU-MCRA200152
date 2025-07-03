#include <windows.h>
#include <iostream>
#include <conio.h>
#include "winio.h"

#pragma comment(lib,"winio.lib")
using namespace std;


int creat_DO(int(&DO_data)[8])
{
	int temp = 0;
	int i = 0;
	for (i = 7; i>0; i--)
	{
		temp = (temp + DO_data[i]) * 2;
	}
	return temp + DO_data[0];
}

int creat_DI(int(&DI_bit)[8], int num)
{
	int i = 0;
	for (i = 0; i<8; i++)
		DI_bit[i] = (num >> i) & 0x0001;
	return 0;
}

int rotate_data()
{
	static int dataout = 1;
	dataout <<= 1;
	if (dataout == 256) dataout = 1;
	return dataout;
}

int blame_data(int dataout)
{
	static int is_light = 0;
	is_light ^= 1;
	return is_light ? dataout : 0;
}

void main(void)
{
	unsigned short BASE_ADDRESS = 0x3F40;
	int ioport = 16;

	// 初始化WinIO 
	if (!InitializeWinIo())
	{
		cout << "Error In InitializeWinIo!" << endl;
		exit(1);
	}
	//数字量输出

	char c;
	int dataout;
	int DO[8] = { 0 };
	int DI[8] = { 0 };
	int stage = 0, cnt = 0;
	DWORD *datain = new DWORD;
	cout << "输入 Q 强制停止！" << endl;
	int stage5cnt=2;
	int rotate = 1;
	while (1)
	{
		if (_kbhit())
		{
			char flag = _getch();
			if (flag == 'q' || flag == 'Q')
			{
				cout << "用户输入退出，退出程序！" << endl;
				Sleep(500);
				break;
			}
		}
		dataout = 0;
		GetPortVal(BASE_ADDRESS + ioport, datain, 1);
		creat_DI(DI, *datain);
		if (stage==0 && (!DI[0]))
		{
			stage = 1;
			dataout |= 1;
		}
		else if (stage == 1) // 注水
		{
			if (!DI[1])
			{

				stage = 2;
				dataout |= (1 << 1);
				cnt = 50;

			}
			else
				dataout |= 1;
		}
		else if (stage == 2) // 浸泡
		{
			cnt--;
			if (!cnt)
			{
				stage = 3;
				cnt = 50;
			}
			else
				dataout |= (1 << 1);
		}
		else if (stage == 3) // 洗涤旋转运行
		{
			cnt--;
			if (!cnt)
			{
				stage = 4;
				dataout |= (1 << 2);
				cnt = 20;
			}
			else
				dataout = rotate_data();
		}
		else if (stage == 4) // 排水
		{
			cnt--;
			if (!cnt)
			{
				stage = 5;
				cnt = 60;
			}
			else
				dataout |= (1 << 2);
		}
		else if (stage == 5) // 脱水
		{
			cnt--;
			if (!cnt)
			{
				if (stage5cnt)
				{
					stage5cnt--;
					stage = 6;
				}
				else
					stage = 8;
			}
			else
				dataout = rotate_data();
		}
		else if (stage==6) // 中途注水
		{
			if (!DI[1])
			{
				stage = 7;
				dataout = rotate_data();
				cnt = 40;
			}
			else
				dataout |= 1;
		}
		else if (stage == 7) // 漂洗
		{
			--cnt;
			if (!cnt)
			{
				stage = 4;
				cnt = 20;
			}
			else
				dataout = rotate_data();
		}
		else if (stage == 8)
			dataout |= (1 << 5);

		if (stage == 5) Sleep(30);
		else Sleep(100);

		if((stage & 1)&&(stage!=1)) SetPortVal(BASE_ADDRESS + ioport, dataout, 1);
		else  SetPortVal(BASE_ADDRESS + ioport, blame_data(dataout), 1);
	}
	SetPortVal(BASE_ADDRESS + ioport, 0, 1);
	ShutdownWinIo();													//关闭WinIO
}