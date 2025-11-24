/*	
 *		
 *		Init_OLED							
 *		Oled_Clean();									
 *		Oled_DispString_6x8(0,Line_0,"  Main Meun");	
 *		Oled_DispString_8x16(100,Line_1,"Hz");       
 *		Oled_DispFloat_0V(80,Line_0,DCVoltage);      
 *		Oled_DispFloat_2V(80,Line_0,DCVoltage);  
*/

#include "OLED.h"


code unsigned char F6x8[][6] =
{
0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
0x00, 0x00, 0x42, 0x7F, 0x40, 0x00,// 1
0x00, 0x42, 0x61, 0x51, 0x49, 0x46,// 2
0x00, 0x21, 0x41, 0x45, 0x4B, 0x31,// 3
0x00, 0x18, 0x14, 0x12, 0x7F, 0x10,// 4
0x00, 0x27, 0x45, 0x45, 0x45, 0x39,// 5
0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
0x00, 0x01, 0x71, 0x09, 0x05, 0x03,// 7
0x00, 0x36, 0x49, 0x49, 0x49, 0x36,// 8
0x00, 0x06, 0x49, 0x49, 0x29, 0x1E,// 9
0x00, 0x00, 0x36, 0x36, 0x00, 0x00,// :
	
0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C,// A
0x00, 0x7F, 0x49, 0x49, 0x49, 0x36,// B
0x00, 0x3E, 0x41, 0x41, 0x41, 0x22,// C
0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C,// D
0x00, 0x7F, 0x49, 0x49, 0x49, 0x41,// E
0x00, 0x7F, 0x09, 0x09, 0x09, 0x01,// F
0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A,// G
0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F,// H
0x00, 0x00, 0x41, 0x7F, 0x41, 0x00,// I
0x00, 0x20, 0x40, 0x41, 0x3F, 0x01,// J
0x00, 0x7F, 0x08, 0x14, 0x22, 0x41,// K
0x00, 0x7F, 0x40, 0x40, 0x40, 0x40,// L
0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F,// M
0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F,// N
0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E,// O
0x00, 0x7F, 0x09, 0x09, 0x09, 0x06,// P
0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
0x00, 0x7F, 0x09, 0x19, 0x29, 0x46,// R
0x00, 0x46, 0x49, 0x49, 0x49, 0x31,// S
0x00, 0x01, 0x01, 0x7F, 0x01, 0x01,// T
0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F,// U
0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F,// V
0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F,// W
0x00, 0x63, 0x14, 0x08, 0x14, 0x63,// X
0x00, 0x07, 0x08, 0x70, 0x08, 0x07,// Y
0x00, 0x61, 0x51, 0x49, 0x45, 0x43,// Z
};


#define high 1
#define low 0
#define Brightness 0xCF
#define X_WIDTH 128
#define Y_WIDTH 64

static void Oled_Write_Command(unsigned char x);
static void I2c_Start(void);
static void I2c_Stop(void);
static void I2c_Write_Byte(unsigned char x);
static void Oled_Write_Byte(unsigned char x);

static void Oled_Set_Pos(unsigned char x,unsigned char y);
static void float32_to_str(unsigned char Num, unsigned char u8Char[]);
       //uint16_t Line, uint16_t Column,float num);


void Oled_DispString_6x8(unsigned char Column, unsigned char Line,unsigned char *ch){
        unsigned char c=0,i=0,j=0;
        while (ch[j]!='\0'){
								if(ch[j]<65)
									c=ch[j]-'0';
								else
                  c =ch[j]-54;
                if(Column>126){Column=0;Line++;}
                Oled_Set_Pos(Column,Line);
                for(i=0;i<6;i++)
                Oled_Write_Byte(F6x8[c][i]);
                Column+=6;
                j++;
        }
}



/*OLEDʾ*/
void Oled_DispNUM(unsigned char Column,unsigned char Line,unsigned char num){
	unsigned char chFloat[20]={'\0'};
	float32_to_str(num,chFloat);
	Oled_DispString_6x8(Column,Line,chFloat);
}

void Oled_DispNUM_2(unsigned char Column,unsigned char Line,unsigned char num){
        unsigned char chFloat[20]={'\0'};
        float32_to_str(num,chFloat);
        if(num<10){
                Oled_DispString_6x8(Column,Line,"0");
                Oled_DispString_6x8(Column+6,Line,chFloat);
        }else{
                Oled_DispString_6x8(Column,Line,chFloat);
        }
}

void Init_OLED(void){
        delay_ms(50);             
        Oled_Write_Command(0xae);//--turn off Oled panel
        Oled_Write_Command(0x00);//---set low column address
        Oled_Write_Command(0x10);//---set high column address
        Oled_Write_Command(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
        Oled_Write_Command(0x81);//--set contrast control register
        Oled_Write_Command(Brightness); // Set SEG Output Current Brightness
        Oled_Write_Command(0xa1);//--Set SEG/Column Mapping    
        Oled_Write_Command(0xc8);//Set COM/Row Scan Direction   
        Oled_Write_Command(0xa6);//--set normal display
        Oled_Write_Command(0xa8);//--set multiplex ratio(1 to 64)
        Oled_Write_Command(0x3f);//--1/64 duty
        Oled_Write_Command(0xd3);//-set display offset  Shift Mapping RAM Counter (0x00~0x3F)
        Oled_Write_Command(0x00);//-not offset
        Oled_Write_Command(0xd5);//--set display clock divide ratio/oscillator frequency
        Oled_Write_Command(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
        Oled_Write_Command(0xd9);//--set pre-charge period
        Oled_Write_Command(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
        Oled_Write_Command(0xda);//--set com pins hardware configuration
        Oled_Write_Command(0x12);
        Oled_Write_Command(0xdb);//--set vcomh
        Oled_Write_Command(0x40);//Set VCOM Deselect Level
        Oled_Write_Command(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
        Oled_Write_Command(0x02);//
        Oled_Write_Command(0x8d);//--set Charge Pump enable/disable
        Oled_Write_Command(0x14);//--set(0x10) disable
        Oled_Write_Command(0xa4);// Disable Entire Display On (0xa4/0xa5)
        Oled_Write_Command(0xa6);// Disable Inverse Display On (0xa6/a7)
        Oled_Write_Command(0xaf);//--turn on Oled panel
        Oled_Fill(0x00); 
        Oled_Set_Pos(0,0);
}
 	

void I2c_Start(void){
        // sda_out;
        // scl_out;
        scl_1;
        sda_1;
        sda_0;
        scl_0;
}

void I2c_Stop(void){
        scl_0;
        sda_0;
        scl_1;
        sda_1;
}

void I2c_Write_Byte(unsigned char x){
        unsigned char i;
        scl_0;
        for(i=0;i<8;i++)
        {
                if(x&0x80)
                        sda_1;
                else
                        sda_0;
                scl_1;
                scl_0;
                x<<=1;
        }
        sda_1;
        scl_1;
        scl_0;
}

void Oled_Write_Byte(unsigned char x){
        I2c_Start();
        I2c_Write_Byte(0x78);
        I2c_Write_Byte(0x40);  
        I2c_Write_Byte(x);
        I2c_Stop();
}
void Oled_Write_Command(unsigned char x){
        I2c_Start();
        I2c_Write_Byte(0x78);
        I2c_Write_Byte(0x00);  
        I2c_Write_Byte(x);
        I2c_Stop();
}
void Oled_Fill(unsigned char x){
        unsigned char i1,j1;
        for(i1=0;i1<8;i1++){
                Oled_Write_Command(0xb0+i1);
                Oled_Write_Command(0x01);
                Oled_Write_Command(0x10);
                for(j1=0;j1<X_WIDTH;j1++)
                        Oled_Write_Byte(x);
        }
}

void Oled_Set_Pos(unsigned char x,unsigned char y){
        Oled_Write_Command(0xb0+y);
        Oled_Write_Command(((x&0xf0)>>4)|0x10);
        Oled_Write_Command((x&0x0f)|0x01);
}
/**********************************************************
*��������  void float32_to_str(float Num, unsigned char n, unsigned char u8Char[])
*���ܣ�    ��һ�������ȸ�����ת��Ϊ�ַ���
*��ڲ�����float Num, unsigned char n, unsigned char u8Char[]
*���ز�������
*˵����    Num:Ԥת�������ݣ�u8Char[]:���ڴ���ת������ַ���
*               n: ��ȷ��С�������λ
***********************************************************/
void float32_to_str(unsigned char Num, unsigned char u8Char[])
{
        unsigned char  u8_digit = 0; //���ݵ�λ��
        unsigned char  i = 0;
        unsigned long int32_temp  = 0;
				unsigned char u8_temp;

				if(Num==0){
					u8Char[0]='0';
					u8Char[1]='\0';
					return;
				}
        int32_temp = Num < 0 ? (-Num) : (Num); 

        
        /*�Ѹ������ӵ�λ��ʼת��Ϊ�ַ���*/
        i = 0;
        u8_digit = 0;
        while (int32_temp != 0)
		    {
           u8Char[i] = '0' + int32_temp % 10; //�����λ����
           int32_temp = int32_temp / 10;
           u8_digit++;//��������λ�����ַ����ĳ���
           i++;
        }
				
        /*���Ϸ���λ*/
        if(Num < 0)
        {
                u8Char[u8_digit] = '-';                      //���ַ���ĩβ���ϸ��ű�־
                u8_digit++;                                  //�ַ������ȼ�1
        }

        /*��u8Char�е��ַ����ߵ�λ�Ե�*/
        for (i=0; i < (u8_digit / 2); i++)
        {
                u8_temp = u8Char[i];
                u8Char[i] = u8Char[u8_digit - i - 1];
                u8Char[u8_digit - i - 1] = u8_temp;
        }
        u8Char[u8_digit] = '\0';                                    //�ַ�����β����һ��'\0'
}