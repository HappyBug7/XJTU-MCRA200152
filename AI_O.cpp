
#include <windows.h>
#include <iostream>
#include <conio.h>
#include "winio.h"

#pragma comment(lib,"winio.lib")

using namespace std;

float get_volt(int iChannel, float fLoVolt, float fHiVolt, unsigned char ucGain);

void set_volt(int iChannel, float fVoltage, float fHiVolt, float fLoVolt);


void main(void)
{



	char c;
	unsigned short BASE_ADDRESS = 0x3F40;
	int iChannel = 0;
	float fHiVolt, fLoVolt, temp, fVoltage;
	unsigned short adData;
	unsigned char ucGain;
	unsigned char ucStatus = 1;

	int i = 1;
	int cnt = 10, num = 5;
	bool valid = false;
	DWORD *p = new DWORD;


	/* 初始化WinIO */
	if (!InitializeWinIo())
	{
		cout << "Error In InitializeWinIo!" << endl;
		exit(1);
	}


	SetPortVal(BASE_ADDRESS + 9, 0, 1);  //Clear FIFO



	//选择起始和结束通道

	SetPortVal(BASE_ADDRESS + 4, iChannel, 1); //Start channel
	SetPortVal(BASE_ADDRESS + 5, iChannel, 1); //Stop channel

	//设置增益及电压范围
	fLoVolt = -10.0;
	fHiVolt = 10.0;
	fVoltage = 5;
	ucGain = 0x04;
	SetPortVal(BASE_ADDRESS + 2, ucGain, 1);


	set_volt(iChannel, fVoltage, fHiVolt, 0);

	int mode = 0;

	cout << "模式 0 为自动测试，模式 1 为 手动测试，请确定模式：";
	cin >> mode;

	if (mode == 1)
	{
		cout << "输入N采样，输入Q退出" << endl;
		while (1)
		{
			if (_kbhit())
			{
				char flag = _getch();
				if (flag == 'q' || flag == 'Q')
				{
					cout << "用户输入退出，程序退出" << endl;
					break;
				}
				else if (flag == 'n' || flag == 'N')
				{
					cout <<endl<< "请输入测试电压:";
					cin >> fVoltage;
					if (fVoltage >= 10 || fVoltage < 0) {
						cout << "错误输入！" << endl;
						valid = false;
						continue;
					}
					valid = true;
					num = 5;
					set_volt(iChannel, fVoltage, fHiVolt, 0);
					float tmp = get_volt(iChannel, fLoVolt, fHiVolt, ucGain);
					cout << endl << endl << "第 " << iChannel << " 通道输出电压为：" << tmp << "V" << endl;
					cout << "绝对误差为：" << abs(tmp - fVoltage) << " 相对误差为：" << 100 * abs(tmp - fVoltage) / fVoltage << "%" << endl;
				}
			}
			else if (valid) {
				if (cnt <= 0 && num) {
					set_volt(iChannel, fVoltage, fHiVolt, 0);
					float tmp = get_volt(iChannel, fLoVolt, fHiVolt, ucGain);
					cout << endl << endl << "第 " << iChannel << " 通道输出电压为：" << tmp << "V" << endl;
					cout << "绝对误差为：" << abs(tmp - fVoltage) << " 相对误差为：" << 100 * abs(tmp - fVoltage) / fVoltage << "%" << endl;
					cnt = 10;
					num--;
				}
				cnt--;
			}
			Sleep(100);
		}
	}
	else
	{
		float max_d_td, max_d=0.0;
		float min_d_td, min_d=9999.9;
		for (float td = 0.128; td <= 9.5; td += 0.2)
		{
			set_volt(iChannel, td, fHiVolt, 0);
			float tmp = get_volt(iChannel, fLoVolt, fHiVolt, ucGain);
			cout << endl<<"当输入为:" << td << " V 时" << endl;
			cout << "第 " << iChannel << " 通道输出电压为：" << tmp << "V" << endl;
			if (100 * abs(tmp - td) / td > max_d)
			{
				max_d = 100 * abs(tmp - td) / td;
				max_d_td = td;
			}
			if (100 * abs(tmp - td) / td < min_d)
			{
				min_d = 100 * abs(tmp - td) / td;
				min_d_td = td;
			}
			cout << "绝对误差为：" << abs(tmp - td) << " 相对误差为：" << 100 * abs(tmp - td) / td << "%" << endl ;
		}
		cout <<endl<< "测试已经结束，一些数据如下：" << endl
			<< "当输入为：" << max_d_td << " V 时，相对误差最大，相对误差为: " << max_d << endl;
		cout << "当输入为：" << min_d_td << " V 时，相对误差最小，相对误差为: " << min_d << endl;
		cout << "输出任意键退出" << endl;
		while (1)
		{
			if (_kbhit())
				break;
		}
	}
	cout << "输入电压值，输出显示输出电压、相对与绝对误差" << endl;
	cout << "输入N采样，输入Q退出" << endl;
	



	ShutdownWinIo();
}

float get_volt(int iChannel, float fLoVolt, float fHiVolt, unsigned char ucGain) {
	char c;
	unsigned short BASE_ADDRESS = 0x3F40;
	float temp;
	unsigned short adData;
	unsigned char ucStatus = 1;

	int i = 1;

	unsigned short tmp;
	DWORD *p = new DWORD;
	SetPortVal(BASE_ADDRESS + 9, 0, 1);  //Clear FIFO

	SetPortVal(BASE_ADDRESS + 2, ucGain, 1);

	SetPortVal(BASE_ADDRESS + 6, 0x01, 1);  // 设置软件触发方式，禁止中断
	SetPortVal(BASE_ADDRESS + 0, 0, 1);  // 软件触发AD

	ucStatus = 1;

	while ((!_kbhit()) && (ucStatus == 1))
	{
		GetPortVal(BASE_ADDRESS + 7, p, 1);
		ucStatus = (*p) & 0x01;
	}


	if (ucStatus == 0)
	{

		GetPortVal(BASE_ADDRESS + 0, p, 2);
		adData = (*p) & 0x0fff;

		temp = (fHiVolt - fLoVolt) * adData / 4095.0 + fLoVolt;
		return temp;

	}
}

void set_volt(int iChannel, float fVoltage, float fHiVolt, float fLoVolt) {
	int BASE_ADDRESS = 0x3F40;
	int AOPort;
	int LByte;
	int HByte;
	char c;
	unsigned short outData;

	SetPortVal(BASE_ADDRESS + 14, 1, 1);  //设置内部参考电压为5V

	AOPort = 10 + iChannel * 2;

	outData = (unsigned short)(fVoltage / (fHiVolt - fLoVolt) * 0xfff);
	LByte = outData & 0xff;
	HByte = (outData >> 8) & 0x0f;

	SetPortVal(BASE_ADDRESS + AOPort, LByte, 1);     //低字节部分
	SetPortVal(BASE_ADDRESS + AOPort + 1, HByte, 1);  //高字节部分
}