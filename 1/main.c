#include <REGX52.H>
#include <OLED.h>
#define uchar unsigned char
// ind:0~4,作为motor_phase_4的index使用
uchar ind = 0;
uchar ind2 = 4;

// x与n0控制Motor速度
int sec_motor = 0;
int min_motor = 0;
int n0 = 220;
// 实际n0
// int n0=30;

// flu控制1s时间加1
uchar flu = 0;

// 控制按键消抖
uchar pre_key = 0xff;
uchar cur_key = 0xff;

// page=0为时间显示页，page=1为设置时间页
uchar page = 0;
// mode=0为LR左右模式，mode=1为UD上下模式
uchar mode = 0;
// sel=0选择为H，sel=1选择为M，sel=2选择为S
uchar sel = 2;
// 时分秒
uchar time[3] = {0, 0, 0};
uchar alarm[3] = {0, 0, 0};

uchar code motor_phase_4[8] = {
	0x01, // A
	0x02, // B
	0x04, // C
	0x08, // D

	0x10, // A
	0x20, // B
	0x40, // C
	0x80  // D
};

void delay_ms(unsigned int ms_number);

void time0() interrupt 1
{
	TH0 = 0xFC;
	TL0 = 0x66;
	if (sec_motor >= n0)
	{
		P2 = (0x0f & motor_phase_4[ind])|(0xf0 & P2);
		ind++;
		if (ind > 3)
		{
			ind = 0;
		}
		sec_motor = 0;
		min_motor++;
	}
	if(min_motor>=n0){
		P2 = (0xf0 & motor_phase_4[ind2])|(0x0f & P2);
		ind2++;
		if (ind2 > 7)
		{
			ind2 = 4;
		}
		min_motor=0;
	}
	sec_motor++;
}

void time1() interrupt 3
{
	TH1 = 0xB1;
	TL1 = 0xE0;
	if(page==0){
		flu++;
		if (flu >= 50)
		{
			time[2]++;
			time[1]+=(time[2]/60);
			time[0]+=(time[1]/60);
			if(time[2]>=60){
				Oled_Clean();
				time[2]%=60;
			}
			time[1]%=60;
			time[0]%=24;
			flu=0;
		}
	}
	cur_key = P1;
	if (cur_key != pre_key)
	{
		if (pre_key == ~0x00)
		{
			if (cur_key == ~0x01)
			{
				// 加速、上、左
				if (page == 0)
				{
					n0 -= 10;
					if (n0 <= 120)
						n0 = 120;
				//if(n0<=10)
				//  n0=10;
				}

				else
				{
					if (mode == 0)
					{
						if (sel == 0)
							sel = 3;
						  sel--;
					}
					else
					{
						time[sel]++;
						if (sel == 0)
						{
							if(time[0]>=24){
								time[0]-=24;
								Oled_Clean();
							}
						}
						else
						{
							if(time[sel]>=60){
								time[sel]-=60;
								Oled_Clean();
							}
						}
					}
				}
			}
			else if (cur_key == ~0x02)
			{
				// 减速、下、右
				if (page == 0)
				{
					n0 += 10;
					if (n0 >= 300)
						n0 = 300;
					//if(n0>=60)
					//		n0=60;
				}
				else
				{
					if (mode == 0)
					{
						sel++;
						if (sel >= 3)
						{
							sel = 0;
						}
					}
					else
					{
						if (time[sel] == 0)
						{
							if (sel == 0)
							{
								time[0] = 23;
							}
							else
							{
								time[sel] = 59;
							}
						}
						else if(time[sel]==10){
							Oled_Clean();
							time[sel]--;
						}else
						{
							time[sel]--;
						}
					}
				}
			}
			else if (cur_key == ~0x04)
			{
				// 按键功能切换，切换LR模式与UD模式
				mode = mode ? 0 : 1;
			}
			else if (cur_key == ~0x08)
			{
				// 切换显示页,显示时转动，不显示时不转动
				TR0 = ~TR0;
				page = page ? 0 : 1;
				Oled_Clean();
			}
		}
		pre_key = cur_key;
	}
}

void main()
{
	TMOD = 0x11;

	TH0 = 0xFC;
	TL0 = 0x66;

	TH1 = 0xB1;
	TL1 = 0xE0;

	ET0 = 1;
	ET1 = 1;
	EA = 1;

	TR0 = 1;
	TR1 = 1;

	Init_OLED();
	Oled_Clean();

	while (1)
	{
		if (page == 0)
		{
			Oled_DispString_6x8(2, 0, "DIS");
			Oled_DispString_6x8(2, 4, "SPEED");
			Oled_DispNUM(50, 4, 400 - n0);
		}
		else
		{
		  Oled_DispString_6x8(2, 0, "SET");
			if(mode==0)Oled_DispString_6x8(50, 0, "LR");
			else  Oled_DispString_6x8(50, 0, "UD");
			Oled_DispString_6x8(2, 4, "SEL");
			if(sel==0){
					Oled_DispString_6x8(50, 4, "H");
			}else if(sel==1){
					Oled_DispString_6x8(50, 4, "M");
			}else{
					Oled_DispString_6x8(50, 4, "S");
			}
		}
			Oled_DispString_6x8(2, 1, "H");
			Oled_DispNUM(15, 1, time[0]);
			Oled_DispString_6x8(2, 2, "M");
			Oled_DispNUM(15, 2, time[1]);
			Oled_DispString_6x8(2, 3, "S");
			Oled_DispNUM(15, 3, time[2]);
			delay_ms(200);
	}
}

void delay_ms(unsigned int ms_number)
{
	unsigned int i;
	unsigned char j;
	for (i = 0; i < ms_number; i++)
	{
		for (j = 0; j < 200; j++)
			;
		for (j = 0; j < 102; j++)
			;
	}
}



