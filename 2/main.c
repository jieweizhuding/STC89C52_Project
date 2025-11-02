/*
说明：串口中断接收'0'字符，返回'R'，仅作为说明用。
*/

#include <REGX52.H>

sbit button = P3 ^ 2;
sbit led0=P2^2;
sbit led1=P2^3;
sbit led2=P2^4;
sbit BZ=P2^5;
unsigned char code LED_7SEG[10] = {
    0x3F,  // 0 - 00111111
    0x06,  // 1 - 00000110
    0x5B,  // 2 - 01011011
    0x4F,  // 3 - 01001111
    0x66,  // 4 - 01100110
    0x6D,  // 5 - 01101101
    0x7D,  // 6 - 01111101
    0x07,  // 7 - 00000111
    0x7F,  // 8 - 01111111
    0x6F   // 9 - 01101111
};
unsigned char l_num = 0;
unsigned int h_num = 0;
unsigned char pre_key=1;
unsigned char cur_key=1;
unsigned char P1_pre_key=0xf7;
unsigned char P1_cur_key=0xf7;
unsigned char time=0;
unsigned char delta_num=0;
unsigned char twinkle_time=20;
int set_num=50;
unsigned char mode=0;
unsigned char store_P2=0xff;
unsigned char beep=0;

void delay(){
	unsigned char a,b;
	for(a=0;a<120;a++){
		for(b=0;b<120;b++);
	}
}

void int1() interrupt 2
{
	EX1=0;
	delay();
	if((P3&0x08)==0){
		beep=beep?0:1;
	}
	EX1=1;
}

void display(int num){	
	if(num/100==0){
		if(led0==0){
			led1=0;
			led0=1;
			P0=LED_7SEG[num/10];
		}else{
			led1=0;
			led0=0;
			P0=LED_7SEG[num%10];
		}
	}else{
		if(led1==1){
			led1=0;
			led0=1;
			P0=LED_7SEG[(num/10)%10];
		}else if(led0==1){
			led1=0;
			led0=0;
			P0=LED_7SEG[num%10];
		}else{
			led1=1;
			led0=0;
			P0=LED_7SEG[num/100];
		}
	}
}

//20ms
void time0() interrupt 1
{
	TH0 = 0xB1;	  // 高8位
	TL0 = 0xE0;	  // 低8位

	//按键脉冲
	cur_key=button;
	if(cur_key==0&&pre_key==1){
		l_num++;
		delta_num++;
		if(l_num>=10){
			l_num=0;
			h_num++;
			P2=h_num;
			P2=~P2;
		}
	}
	pre_key=cur_key;

	//1s检测
	time++;
	if(time>=50){
		delta_num=0;
		time=0;
		//在闪烁时再次触发闪烁条件可能导致LED显示颠倒
		if(twinkle_time%2==1){
			P2=~P2;
		}
		twinkle_time=20;
	}
	if(delta_num>2&&twinkle_time>0){
		P2=~P2;
		twinkle_time--;
	}

	P1_cur_key=P1;
	if(P1_cur_key!=0xf7&&P1_pre_key==0xf7){
		switch(P1_cur_key){
			case 0x77:{
				mode=mode?0:1;
				if(mode==1){
					store_P2=P2;
					P2=0xff;
					led2=0;
				}else{
					P2=store_P2;
					P0=0;
				}
				break;
			};
			case 0xb7:{
				set_num+=50;
				if(set_num>500)set_num=500;
				break;
			};
			case 0xd7:{
				set_num-=50;
				if(set_num<50)set_num=50;
				break;
			};
		}
	}
	if(mode==1){
		display(set_num);
	}
	P1_pre_key=P1_cur_key;
}

void UART_ISR(void) interrupt 4
{
    unsigned char received_data;
    
    if(RI == 1)
    {
        RI = 0;  // 清除接收中断标志
        received_data = SBUF;
        
        if(received_data=='0'){
			SBUF='R';
		}
    }
    
    if(TI == 1)
    {
        TI = 0;  // 清除发送中断标志
    }
}


void main()
{
	TMOD &= 0xF0;    // 清除定时器0模式位
    TMOD |= 0x01;    // 定时器0，模式1
	
	TMOD &= 0x0F;    // 清除定时器1模式位
    TMOD |= 0x20;    // 定时器1，模式2
	
	SCON = 0x50;     // 模式1，8位UART，允许接收
	TH0 = 0xB1;	  // 高8位
	TL0 = 0xE0;	  // 低8位

	TH1 = 0xFD;      // 9600波特率（11.0592MHz）
    TL1 = 0xFD;

	TR0 = 1;         // 启动定时器0
	TR1 = 1; // 启动定时器1

	EA = 1;	 // 开启总中断

	ET0=1;

	IT1 = 1;
	EX1 = 1;

    ES = 1;          // 串口中断允许

	// unsigned char i=0;
	P0 =0x00;
	P1=0xF7;

	P2=0xff;

	while (1)
	{
		if(beep){
			BZ=~BZ;
			delay();
		}else{
			BZ=0;
		}
	}
}