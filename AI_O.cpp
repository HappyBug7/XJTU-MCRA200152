
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


	/* ��ʼ��WinIO */
	if (!InitializeWinIo())
	{
		cout << "Error In InitializeWinIo!" << endl;
		exit(1);
	}


	SetPortVal(BASE_ADDRESS + 9, 0, 1);  //Clear FIFO



	//ѡ����ʼ�ͽ���ͨ��

	SetPortVal(BASE_ADDRESS + 4, iChannel, 1); //Start channel
	SetPortVal(BASE_ADDRESS + 5, iChannel, 1); //Stop channel

	//�������漰��ѹ��Χ
	fLoVolt = -10.0;
	fHiVolt = 10.0;
	fVoltage = 5;
	ucGain = 0x04;
	SetPortVal(BASE_ADDRESS + 2, ucGain, 1);


	set_volt(iChannel, fVoltage, fHiVolt, 0);

	int mode = 0;

	cout << "ģʽ 0 Ϊ�Զ����ԣ�ģʽ 1 Ϊ �ֶ����ԣ���ȷ��ģʽ��";
	cin >> mode;

	if (mode == 1)
	{
		cout << "����N����������Q�˳�" << endl;
		while (1)
		{
			if (_kbhit())
			{
				char flag = _getch();
				if (flag == 'q' || flag == 'Q')
				{
					cout << "�û������˳��������˳�" << endl;
					break;
				}
				else if (flag == 'n' || flag == 'N')
				{
					cout <<endl<< "��������Ե�ѹ:";
					cin >> fVoltage;
					if (fVoltage >= 10 || fVoltage < 0) {
						cout << "�������룡" << endl;
						valid = false;
						continue;
					}
					valid = true;
					num = 5;
					set_volt(iChannel, fVoltage, fHiVolt, 0);
					float tmp = get_volt(iChannel, fLoVolt, fHiVolt, ucGain);
					cout << endl << endl << "�� " << iChannel << " ͨ�������ѹΪ��" << tmp << "V" << endl;
					cout << "�������Ϊ��" << abs(tmp - fVoltage) << " ������Ϊ��" << 100 * abs(tmp - fVoltage) / fVoltage << "%" << endl;
				}
			}
			else if (valid) {
				if (cnt <= 0 && num) {
					set_volt(iChannel, fVoltage, fHiVolt, 0);
					float tmp = get_volt(iChannel, fLoVolt, fHiVolt, ucGain);
					cout << endl << endl << "�� " << iChannel << " ͨ�������ѹΪ��" << tmp << "V" << endl;
					cout << "�������Ϊ��" << abs(tmp - fVoltage) << " ������Ϊ��" << 100 * abs(tmp - fVoltage) / fVoltage << "%" << endl;
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
			cout << endl<<"������Ϊ:" << td << " V ʱ" << endl;
			cout << "�� " << iChannel << " ͨ�������ѹΪ��" << tmp << "V" << endl;
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
			cout << "�������Ϊ��" << abs(tmp - td) << " ������Ϊ��" << 100 * abs(tmp - td) / td << "%" << endl ;
		}
		cout <<endl<< "�����Ѿ�������һЩ�������£�" << endl
			<< "������Ϊ��" << max_d_td << " V ʱ�����������������Ϊ: " << max_d << endl;
		cout << "������Ϊ��" << min_d_td << " V ʱ����������С��������Ϊ: " << min_d << endl;
		cout << "���������˳�" << endl;
		while (1)
		{
			if (_kbhit())
				break;
		}
	}
	cout << "�����ѹֵ�������ʾ�����ѹ�������������" << endl;
	cout << "����N����������Q�˳�" << endl;
	



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

	SetPortVal(BASE_ADDRESS + 6, 0x01, 1);  // �������������ʽ����ֹ�ж�
	SetPortVal(BASE_ADDRESS + 0, 0, 1);  // �������AD

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

	SetPortVal(BASE_ADDRESS + 14, 1, 1);  //�����ڲ��ο���ѹΪ5V

	AOPort = 10 + iChannel * 2;

	outData = (unsigned short)(fVoltage / (fHiVolt - fLoVolt) * 0xfff);
	LByte = outData & 0xff;
	HByte = (outData >> 8) & 0x0f;

	SetPortVal(BASE_ADDRESS + AOPort, LByte, 1);     //���ֽڲ���
	SetPortVal(BASE_ADDRESS + AOPort + 1, HByte, 1);  //���ֽڲ���
}