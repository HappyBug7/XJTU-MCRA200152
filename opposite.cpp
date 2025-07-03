// 数字量输入例

#include <windows.h>
#include <iostream>

#include "winio.h"
#include <conio.h>

#pragma comment(lib,"winio.lib")
using namespace std;


int creat_DI(int (&DI_bit)[8], int num)
{
  	int i=0;
  	for(i=0;i<8;i++)
   	DI_bit[i]=(num>>i)&0x0001;
  	return 0;
}

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

void main(void)
{
	char c;
	unsigned short BASE_ADDRESS = 0x3F40;
	int iPort=16;
	int oPort = 16;
	if (!InitializeWinIo())
		cout<<"Error In InitializeWinIo!"<<endl;
	int i;
	char DI_data ;
	DWORD *p = new DWORD;
	int DI[8]={0};
	int mode = 0;
	cout << "输入 Q 退出程序，输入 S 切换模式" << endl;
    while(1)
    {
		GetPortVal(BASE_ADDRESS + iPort,p,1);
		DI_data = *p;
		creat_DI(DI, DI_data);
		Sleep(10);
		reverse(DI, DI + 8);
		if(!mode) SetPortVal(BASE_ADDRESS + oPort, creat_DO(DI), 1);
		else SetPortVal(BASE_ADDRESS + oPort, 255-creat_DO(DI), 1);
		if (_kbhit())
		{
			char flag = _getch();
			if (flag == 'q' || flag == 'Q')
			{
				cout << "用户输入退出，退出程序！" << endl;
				Sleep(500);
				break;
			}
			else if (flag == 's' || flag == 'S')
			{
				if(mode==1) cout << "切换模式！进入对管模式" << endl;
				else cout << "切换模式！进入对管取反模式" << endl;
				mode ^= 1;
			}
		}
   }
   SetPortVal(BASE_ADDRESS + oPort, 0, 1);
   ShutdownWinIo();		//关闭WinIO	
	
}

