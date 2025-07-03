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

	int num = 0;
	int i;
	char DI_data ;
	DWORD *p = new DWORD;
	int flag[2] = { 0 };
	int cnt = 79;
	int mode = 0;

	int DI[8] = { 0 }, data_out = 0;
	cout << "输入Q退出！" << endl;

    while(1)
    {
		GetPortVal(BASE_ADDRESS + ioPort,p,1);
		DI_data = *p;
		creat_DI(DI, DI_data);
		data_out = 0;

		if (!DI[0]){
			if (!flag[0]){
				flag[0] = 1;
				num++;
				cout << "当前已装总产品数为" << num << endl;
			}
		}
		else{
			flag[0] = 0;
		}
		
		if (!DI[1]){
			if (!flag[1] && num >= 8){
				num -= 8;
				cout << "开始打包, 剩余产品数为" << num << endl;
				mode = 1;
			}
			else if (!flag[1]){
				cout << "不满足打包条件" << endl;
			}
			flag[1] = 1;
		}
		else{
			flag[1] = 0;
		}

		if (!mode){
			if (!DI[0])
				data_out = 1 << ((num - 1) % 8);
			else
				data_out = 0;
		}
		else{
			data_out = 1 << (cnt / 10);
			if (cnt == 0){
				cnt = 79;
				mode = 0;
			}
			cnt--;
		}

		SetPortVal(BASE_ADDRESS + ioPort, data_out,1);
		Sleep(30);

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

