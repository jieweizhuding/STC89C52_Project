#include <REGX52.H>
#include <OLED.h>
#define uchar unsigned char

/*
需要
1. 开关电机  电机开时无法设置计时，电机开时只能通过按键关闭电机或远程关闭电机
2. 电机关时，可以设置电机的定时时间，电机的模式，可以按键设置或远程设置

确定  page切换
取消  page=1


*/
// PWM是否启动
sbit PWM = P2 ^ 0;
uchar PWM_s = 0;
uchar PWM_mode = 1;
uchar PWM_time = 0;
uchar set_time = 50;

// 定时结束,0代表未结束，1代表结束
sbit BZ = P2 ^ 1;
uchar alarm = 0;

// flu实现1s1次的“中断”
unsigned int flu = 0;

// 控制按键消抖
uchar pre_key = 0xff;
uchar cur_key = 0xff;

// page=0为电机启动页，显示倒计时，page=1为设置时间、电机模式页
uchar page = 1;
// mode=0为LR左右模式，mode=1为UD上下模式
uchar mode = 0;
// sel=0选择为M，sel=1选择为S
uchar sel = 0;
// 分秒
uchar time[2] = {0, 0};

void delay_ms(unsigned int ms_number);

void time0() interrupt 1
{
	TH0 = 0xB8; // 20ms 初值高位
	TL0 = 0x00; // 20ms 初值低位

	if (page == 0 && alarm == 0)
	{
		flu++;
		if (flu >= 50)
		{
			time[1]--;
			// if (time[1] == 9){
			// 	Oled_Clean();
			// 	Oled_Clean();
			// }
			if (time[1] >= 60)
			{
				time[0]--;
				if (time[0] >= 100)
				{
					// TODO: 定时归零
					time[0] = 11;
					time[1] = 11;
					alarm = 1;
					PWM_s=0;
				}
				else
				{
					time[1] = 59;
				}
				// if (time[0] == 9){
				// 	Oled_Clean();
				// 	Oled_Clean();
				// }
			}
			flu = 0;
		}
	}

	cur_key = P1;
	if (cur_key != pre_key)
	{
		if (pre_key == ~0x00)
		{
			if (cur_key == ~0x01 || cur_key == ~0x02)
			{
				if (page)
				{
					if (mode == 0)
					{
						sel = sel ? 0 : 1;
					}
					else
					{
						if (cur_key == ~0x01)
						{
							if (mode == 1)
							{
								time[sel]++;
								// sel=1,最大为59，sel=0，最大为99
								if (time[sel] > 59 + (1 - sel) * 40)
								{
									time[sel] = 0;
									// Oled_Clean();
									// Oled_Clean();
								}
							}
							else
							{
								if (PWM_mode != 2)
								{
									PWM_mode++;
									set_time += 30;
								}
							}
						}
						else
						{
							if (mode == 1)
							{
								if (time[sel] == 0)
								{
									time[sel] = 59 + (1 - sel) * 40;
								}
								else
								{
									// if (time[sel] == 10)
									// {
									// 	Oled_Clean();
									// 	Oled_Clean();
									// }
									time[sel]--;
								}
							}
							else
							{
								if (PWM_mode != 0)
								{
									PWM_mode--;
									set_time -= 30;
								}
							}
						}
					}
				}
			}
			else if (cur_key == ~0x04)
			{
				// 按键功能切换，切换LR模式与UD模式
				mode++;
				if (mode > 2)
				{
					mode = 0;
				}
			}
			else if (cur_key == ~0x08)
			{
				// 切换显示页,显示时转动，不显示时不转动
				page = page ? 0 : 1;
				alarm = 0;
				if(page==0){
					PWM_s = 1;
				}else{
					PWM_s=0;
				}
				flu = 0;

				Oled_Clean();
				Oled_Clean();
			}
		}
		pre_key = cur_key;
	}
}

// 1ms
void time1() interrupt 3
{
	TH1 = 0xFC;
	TL1 = 0x66;
	if (PWM_s)
	{
		if (PWM_time < set_time)
		{
			PWM = 1;
		}
		else
		{
			PWM = 0;
		}
		PWM_time++;
		if (PWM_time >= 100)
			PWM_time = 0;
	}
	else
	{
		PWM = 0;
	}
}

void main()
{
	TMOD = 0x11;

	TH0 = 0xB8; // 20ms 初值高位
	TL0 = 0x00; // 20ms 初值低位

	TH1 = 0xFC;
	TL1 = 0x66;

	ET0 = 1;
	ET1 = 1;
	EA = 1;

	TR0 = 1;
	TR1 = 1;

	Init_OLED();
	Oled_Clean();

	while (1)
	{
		BZ = alarm ? 0 : 1;
		if (page == 0)
		{
			if (alarm)
				Oled_DispString_6x8(2, 0, "A");
			else
				Oled_DispString_6x8(2, 0, "W");
		}
		else
		{
			Oled_DispString_6x8(2, 0, "SET");
			if (mode == 0)
				Oled_DispString_6x8(50, 0, "LR");
			else if (mode == 1)
				Oled_DispString_6x8(50, 0, "UD");
			else
				Oled_DispString_6x8(50, 0, "SP");
			Oled_DispString_6x8(2, 4, "SEL");
			if (sel == 0)
			{
				Oled_DispString_6x8(50, 4, "M");
			}
			else
			{
				Oled_DispString_6x8(50, 4, "S");
			}
		}
		Oled_DispString_6x8(2, 2, "M");
		Oled_DispNUM_2(15, 2, time[0]);
		Oled_DispString_6x8(2, 3, "S");
		Oled_DispNUM_2(15, 3, time[1]);
		if (PWM_mode == 0)
			Oled_DispString_6x8(2, 5, "LOW");
		else if (PWM_mode == 1)
			Oled_DispString_6x8(2, 5, "MED");
		else
			Oled_DispString_6x8(2, 5, "HIG");
		Oled_DispNUM_2(50, 5, set_time);
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
