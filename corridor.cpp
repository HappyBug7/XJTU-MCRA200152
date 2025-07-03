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


void main(void)
{
	char c;
	unsigned short BASE_ADDRESS = 0x3F40;
	int ioPort=16;


// 初始化WinIO 
	
	if (!InitializeWinIo())
	{
		cout<<"Error In InitializeWinIo!"<<endl;
		//exit(1);
	}

//数字量输入


	int i;
	char DI_data ;
	DWORD *p = new DWORD;
	

	int DI[8] = { 0 }, data_out;
	int cnt[8] = { 0 };
	cout << "输入Q退出！" << endl;
    while(1)
    {
		GetPortVal(BASE_ADDRESS + ioPort,p,1);
		DI_data = *p;

		creat_DI(DI, DI_data);
		data_out = 0;
		for(i=0;i<8;i++)
		{
			if (!DI[i]) cnt[i] = 50;
			cnt[i] = max(cnt[i] - 1, 0);
			if (cnt[i]) data_out|=(1<<(7-i)); 
		}
		SetPortVal(BASE_ADDRESS + ioPort, data_out,1);
		Sleep(100);
		if (_kbhit())
		{
			char flag = _getch();
			if (flag == 'q' || flag == 'Q')
			{
				cout << "用户要求退出，执行退出!" << endl;
				break;
			}
		}
    }
	SetPortVal(BASE_ADDRESS + ioPort, 0, 1);
    ShutdownWinIo();		//关闭WinIO	
	
}

