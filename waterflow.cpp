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

void Init(int mode, int &cnt, int &dl, int &mark, int &dO)
{
	if (mode == 0)
	{
		cout << "进入模式 0：加速闪烁灯" << endl;
		mode = 0;
		cnt = -1;
		dl = 1000;
	}
	else if (mode == 1)
	{
		cout << "进入模式 1：扩散闪烁灯" << endl;
		mode = 1;
		cnt = 0;
	}
	else if (mode == 2)
	{
		cout << "进入模式 2：步进闪烁灯" << endl;
		mode = 2;
		cnt = 0;
		dO = 255;
		mark = 0;
	}
	return;
}

void main(void)

{
	int mode = 0;
	int dl = 1000;
	int dO;
	int mark = 0;

	unsigned short BASE_ADDRESS = 0x3F40;
	int OPort = 16;

	// 初始化WinIO 
	if (!InitializeWinIo())
	{
		cout << "Error In InitializeWinIo!" << endl;
		exit(1);
	}
	//数字量输出

	char c;
	int DO_data;
	int DO[8] = { 0 };
	int DI[8] = { 0 };
	DO[0] = 1;
	int cnt = -1;
	bool stop = false; 
	cout << "输入 Q 退出程序，输入数字选择下列模式：" << endl
		<< "0: 加速闪烁灯" << endl
		<< "1: 扩散闪烁灯" << endl
		<< "2: 步进闪烁灯" << endl
		<< "3: 加速闪烁灯" << endl;
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
		DWORD *p=new DWORD;
		GetPortVal(BASE_ADDRESS + OPort, p, 1);
		int en = *p & 0x01;
		if (!en) 
			continue;
		creat_DI(DI, *p);
		for (int i = 1; i < 7;++i)
			if (!DI[i]){
				if (mode == i-1) break;
				else
				{
					mode = i-1;
					Init(mode, cnt, dl, mark, dO);
					for (int i = 0; i < 8; ++i)
						DO[i] = 0;
					break;
				}
			}
		switch(mode){
			case 0:
				Sleep(dl);
			    DO[(cnt + 1) % 8] = 1;
				DO[cnt] = 0;
				DO_data = creat_DO(DO);
				SetPortVal(BASE_ADDRESS + OPort, DO_data, 1);
				cnt = (cnt + 1) % 8;
				dl -= 100;
				if (cnt == 0)
					dl = 1000;
				break;
			case 1:
			{
				switch (cnt)
				{
					case 0:
						SetPortVal(BASE_ADDRESS + OPort, 24, 1);
						break;
					case 1:
						SetPortVal(BASE_ADDRESS + OPort, 36, 1);
						break;
					case 2:
						SetPortVal(BASE_ADDRESS + OPort, 66, 1);
						break;
					case 3:
						SetPortVal(BASE_ADDRESS + OPort, 129, 1);
						break;
					case 4:
						SetPortVal(BASE_ADDRESS + OPort, 66, 1);
						break;
					case 5:
						SetPortVal(BASE_ADDRESS + OPort, 36, 1);
						break;
					case 6:
						SetPortVal(BASE_ADDRESS + OPort, 24, 1);
						break;
				}
				cnt = (cnt + 1) % 7;
				Sleep(500);
				break;
			}
			case 2:
			{
				SetPortVal(BASE_ADDRESS + OPort, dO, 1);
				Sleep(500);
				if (mark == 0)
					dO = dO >> 1;
				else
					dO = (dO << 1) + 1;
				cnt = (cnt + 1) % 8;
				if (cnt == 0)
					mark = !mark;
				break;
			}

		}
	}
	SetPortVal(BASE_ADDRESS + OPort, 0, 1);
	ShutdownWinIo();													//关闭WinIO
}