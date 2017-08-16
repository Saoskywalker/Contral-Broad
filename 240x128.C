/*ģ��ʱ��*/
//#include<reg51.h>
#include<STC12C5A60S2.h>
#include "uart.h"
extern void delayus(unsigned int n);


sbit beep=P4^4;
sbit shaping_DCF = P0^0;  //����ˮ������ͨ��ŷ�
sbit sonic_DCF   = P0^1;  //������ǹ��ͨ��ŷ�
sbit pump        = P0^2;  //��������
sbit s1_s2       = P0^3;  //����ˮƿS1,S2
sbit wate_sw =P2^0; //ˮλ���

sbit LED_B =P0^6;  //S1ˮƿ��  �͵�ƽ��Ч
sbit LED_G =P0^7;  //S2ˮƿ��  �͵�ƽ��Ч

unsigned int  wate_time=0; //ˮλ����ʱ
unsigned int  beep_time=0;
unsigned char bee_delay=0;
bit beep_sw=0;
bit FS_ONE=0;

bit  china_english=0; //��Ӣ�Ľ���
char CE_buffer=0;
char cut_buff=0;      //��Ӣ�Ľ���
char minute=30,second=0;
char Page=0;
char on_off=0; 
bit time_out=0;
bit deal_tm_bit=0;
char TT_bit=0;
char key_bit=0;
char vac_mode=0;
char RF=0;
char cooling=0;
char RF1=0;
char Get_Date[4];	 //3:2 0040��ַ 1:0  30��00����
char Buffer[20];
char *pp=Buffer;
char xx=0;
char DD=0;
char W_OR_R=0;
char count_RF=0;
int  time_count=0;

void delayus(unsigned int n)
{
	unsigned int i=0,j=0;
	for(;i<n;i++)
		for(;j<124;j++); 
}
void BeepLong()
{
	beep=0;
	delayus(40000);
	beep=1;
	delayus(40000);
	beep=0;
	delayus(40000);
	beep=1;
	delayus(40000);
	beep=0;
	delayus(40000);
	beep=1;
}

void BEEP()
{
	if(!beep)
	{
		bee_delay++;	   //?????	
	}
	if(bee_delay>=100)	   //????????
	{	   
		beep=1;		
		bee_delay=0;							
	}	
}
//��������һ��
void BEEP_wate()
{
	if(++beep_time>40000){beep_time=0;}
	if(beep_time>=35000)	   //????????
	{	   
		beep=0;		
	}	
	else{ beep=1;}	 
}

void Write_date(char address1,char address2,char usedata1,char usedata2){

	//SBUF=0x00;
	//  while(!TT_bit);
	// TT_bit=0;
	TT_bit=0;
	SBUF=0x5A;

	while(!TT_bit);
	TT_bit=0;
	SBUF=0xA5;
	while(!TT_bit);
	TT_bit=0;
	SBUF=0x05;
	while(!TT_bit);
	TT_bit=0;
	SBUF=0x82;
	while(!TT_bit);
	TT_bit=0;
	SBUF=address1;
	while(!TT_bit);
	TT_bit=0;
	SBUF=address2;
	while(!TT_bit);
	TT_bit=0;
	SBUF=usedata1;
	while(!TT_bit);
	TT_bit=0;
	SBUF=usedata2;
	while(!TT_bit);
	TT_bit=0;
}
void sendOneByte(char t)
{
	TT_bit=0;
	SBUF = t;
	while(!TT_bit);
}
void cutP_work_stop(char pic_id)
{

	//5aa5 15 82 00b0  00 06 0001 0000 01 00 01 00 03 00 02 00 01 00 01 00

	sendOneByte(0X5AA5/256); 
	sendOneByte(0X5AA5%256);
	sendOneByte(21);
	sendOneByte(0x82);
	sendOneByte(0);
	sendOneByte(0x70);	   //0xB0��ʾ����ͼ��ĵ�ַ 
	sendOneByte(0);
	sendOneByte(6);		  //0006����ͼƬ���С�ճ��
	sendOneByte(0);
	sendOneByte(1);		  //ֻ��1�����ݰ�
	sendOneByte(0);
	sendOneByte(pic_id);   //ճ����pic_idҳ��ͼƬ	5/7
	sendOneByte(70/256);
	sendOneByte(70%256);	
	sendOneByte(470/256);
	sendOneByte(470%256);	
	sendOneByte(180/256);
	sendOneByte(180%256);	
	sendOneByte(570/256);
	sendOneByte(570%256);
	sendOneByte(70/256);
	sendOneByte(70%256);	
	sendOneByte(470/256);
	sendOneByte(470%256);
	TT_bit=0;
}
void cutP_S1_S2(char pic_id)
{

	//5aa5 15 82 00b0  00 06 0001 0000 01 00 01 00 03 00 02 00 01 00 01 00

	sendOneByte(0X5AA5/256); 
	sendOneByte(0X5AA5%256);
	sendOneByte(21);
	sendOneByte(0x82);
	sendOneByte(0);
	sendOneByte(0x90);	   //0xB0��ʾ����ͼ��ĵ�ַ 
	sendOneByte(0);
	sendOneByte(6);		  //0006����ͼƬ���С�ճ��
	sendOneByte(0);
	sendOneByte(1);		  //ֻ��1�����ݰ�
	sendOneByte(0);
	sendOneByte(pic_id);   //ճ����pic_idҳ��ͼƬ	5/7
	sendOneByte(500/256);
	sendOneByte(500%256);	
	sendOneByte(385/256);
	sendOneByte(385%256);	
	sendOneByte(700/256);
	sendOneByte(700%256);	
	sendOneByte(485/256);
	sendOneByte(485%256);
	sendOneByte(500/256);
	sendOneByte(500%256);	
	sendOneByte(385/256);
	sendOneByte(385%256);	
	TT_bit=0;
}
void cutP_wate_HL(char pic_id)  //ˮλ���ͼ��
{

	//5aa5 15 82 00b0  00 06 0001 0000 01 00 01 00 03 00 02 00 01 00 01 00

	sendOneByte(0X5AA5/256); 
	sendOneByte(0X5AA5%256);
	sendOneByte(21);
	sendOneByte(0x82);
	sendOneByte(0);
	sendOneByte(0x80);	   //0xB0��ʾ����ͼ��ĵ�ַ 
	sendOneByte(0);
	sendOneByte(6);		  //0006����ͼƬ���С�ճ��
	sendOneByte(0);
	sendOneByte(1);		  //ֻ��1�����ݰ�
	sendOneByte(0);
	sendOneByte(pic_id);   //ճ����pic_idҳ��ͼƬ	5/7
	sendOneByte(505/256);
	sendOneByte(505%256);	

	sendOneByte(489/256);
	sendOneByte(489%256);	

	sendOneByte(782/256);
	sendOneByte(782%256);	

	sendOneByte(587/256);
	sendOneByte(587%256);

	sendOneByte(505/256);
	sendOneByte(505%256);	
	sendOneByte(489/256);
	sendOneByte(489%256);	
	TT_bit=0;
}
/**************����ָ�������Ľ�����ת*******/
void jumpPageTo(unsigned char j)
{
	sendOneByte(0X5AA5/256);//��ת����������--03��ҳ��	
	sendOneByte(0X5AA5%256); 
	sendOneByte(0x04);
	sendOneByte(0x80);
	sendOneByte(0x03);
	sendOneByte(0x00);
	sendOneByte(j); 
}  
void GoToVideo(){

	TT_bit=0;
	SBUF=0xaa;

	while(!TT_bit);
	TT_bit=0;
	SBUF=0xbb;
	while(!TT_bit);
	TT_bit=0;
	SBUF=0x01;
	while(!TT_bit);
	TT_bit=0;
	SBUF=0x05;
	while(!TT_bit);
	TT_bit=0;

}

void ExitVideo(){
	TT_bit=0;
	SBUF=0xaa;

	while(!TT_bit);
	TT_bit=0;
	SBUF=0xbb;
	while(!TT_bit);
	TT_bit=0;
	SBUF=0x01;
	while(!TT_bit);
	TT_bit=0;
	SBUF=0x01;
	while(!TT_bit);
	TT_bit=0;
	//	minute=30;second=0;
	//	 Write_date(0x00,0x10,0x00,0x00);
	//	 Write_date(0x00,0x20,0x00,0x00);
	//	 Write_date(0x00,0x30,0x00,0x00);
	//	 Write_date(0x00,0x40,0x00,30);
	//	 Write_date(0x00,0x50,0x30,0x00);
}


void dis_time(){
	char MT[2]; 
	MT[0]=((minute/10)<<4)+minute%10;

	MT[1]=((second/10)<<4)+second%10;

	Write_date(0x00,0x40,MT[0],MT[1]);

}
void deal_tm()
{	 
	deal_tm_bit=0;
	if(--second<0)
	{
		if(--minute>=0){second=59; Write_date(0x00,0x40,minute,0);}
		else {on_off=0;
		cutP_work_stop(3+cut_buff);	
		cutP_S1_S2(3);	
		BeepLong();time_out=1;	S2BUF=0xa0;
		//	Write_date(0x00,0x10,0x00,0x00);
		Write_date(0x00,0x40,0x30,0);
		Write_date(0x00,0x41,0x00,30);
		Write_date(0x00,0x50,0x00,0x00);
		Write_date(0x00,0x60,0x00,0x00);
		Write_date(0x00,0x70,0x00,0x00);
		}
	}
	if(time_out==0)
		dis_time();
}
//=================================================================================
void OPEN_picture()
{
	int key=0;
	CE_buffer=0;   //�����Ӣ�Ľ�����ֵ
	cut_buff=0;   //�������ͼ��������ֵ
	shaping_DCF=1;  //��ˮ�����л���ŷ�
	sonic_DCF  =1;	//����ǹ�л���ŷ�	
	LED_B=1;LED_G=1;	//��ҳ�������ҳ��ر�S1��S2ˮƿ��
	wate_time=0;FS_ONE=0; beep_sw=0;
	on_off=0;RF=0;cooling=0;	
	while(1)
	{
		if(key_bit)
		{
			key_bit=0;
			key=((int)Get_Date[3]<<8)+Get_Date[2];
			switch(key){
			case 0x0000: CE_buffer = Get_Date[0];
				Page = 0x30;  
				if(CE_buffer==1){ cut_buff=12;}
				else            { cut_buff=0;}																							 
				return;
				break;
			}
		}

	}
}
//=================================================================================
void main_picture()
{
	int key=0;
	shaping_DCF=1;  //��ˮ�����л���ŷ�
	sonic_DCF  =1;	//����ǹ�л���ŷ�	
	wate_time=0;FS_ONE=0; beep_sw=0;
	on_off=0;RF=0;cooling=0;	
	LED_B=1;LED_G=1;	//��ҳ�������ҳ��ر�S1��S2ˮƿ��
	while(1)
	{
		if(key_bit){
			key_bit=0;
			key=((int)Get_Date[3]<<8)+Get_Date[2];
			switch(key){
			case 0x0000:Page=Get_Date[0];return;break;
			}
		}
	}
}
//====================================================================================
void Aqua_picture()	  //01ҳ��  ˮ����ҳ��
{
	int key=0;
	minute=30;second=0;
	wate_time=0;FS_ONE=0; beep_sw=0;
	on_off=0;RF=0;cooling=0;
	shaping_DCF=0;  //��ˮ�����л���ŷ�
	sonic_DCF  =1;	//����ǹ�л���ŷ�
	LED_B=0;LED_G=1;	//�ϵ�Ĭ�Ͽ�ͨS1ˮƿ��
	cutP_work_stop(3+cut_buff);  //����������ͣ��Ǹ�λ
	delayus(1000);	
	cutP_S1_S2(3); //����S1,S2ˮƿ��λ
	delayus(1000);	
	cutP_wate_HL(3+cut_buff);  //����ˮλ������λ
	delayus(1000);	
	Write_date(0x00,0x40,0x30,0);
	Write_date(0x00,0x41,0x00,30);
	Write_date(0x00,0x50,0x00,0x00);

	while(1)
	{  // 

		if(deal_tm_bit)
			deal_tm();
		if(time_out){time_out=0;minute=30;second=0;on_off=0;RF=0;cooling=0;}
		if(FS_ONE) 
		{ cutP_wate_HL(11+cut_buff); 
		S2BUF=0xe0;FS_ONE=0;shaping_DCF=1;
		if(CE_buffer==1){ cutP_work_stop(29);} //����������ͣ
		else            { cutP_work_stop(27);} //����Ӣ����ͣ 
		}
		if(beep_sw){ BEEP_wate();} 
		else{ cutP_wate_HL(3+cut_buff); }     //���ˮλ����
		if(key_bit){
			key_bit=0;
			key=(Get_Date[3]<<8)+(int)Get_Date[2]&0x00ff;
			switch(key){
			case 0x00f0: if(!beep_sw){ on_off=Get_Date[0]&0x0f;}
						 if((on_off)&&(!wate_sw)){beep_sw=0;FS_ONE=0;cutP_wate_HL(3);S2BUF=0xe1;shaping_DCF=0;}
						 else{ S2BUF=0xe0;shaping_DCF=1;} 									 
						 if(on_off){ pump=0;if(CE_buffer==1){ cutP_work_stop(28);}else{ cutP_work_stop(26);}}
						 else      { pump=1;if(CE_buffer==1){ cutP_work_stop(29);}else{ cutP_work_stop(27);}}
						 break;	 //����0XE0������
			case 0x0041:minute=Get_Date[0];dis_time();break;
			case 0x0010:s1_s2=1;cutP_S1_S2(3);S2BUF=0x20;LED_B=0;LED_G=1;break; 
			case 0x0020:s1_s2=0;cutP_S1_S2(11);S2BUF=0x21;LED_B=1;LED_G=0;break; 						 
			case 0x00a0:S2BUF=0xa0;delayus(1000);on_off=0;RF=0;cooling=0;
				wate_time=0;beep_sw=0;LED_B=1;LED_G=1;
				s1_s2=1;pump=1;shaping_DCF=1;sonic_DCF=1;Page=0x30;return;break;
			}

		}

	}

}
//==================================================================================================================================
void Oxygun_picture()	  //07ҳ��  ����ǹ  �����л���ŷ�
{
	int key=0;

	minute=30;second=0;
	wate_time=0;FS_ONE=0; beep_sw=0;
	on_off=0;RF=0;cooling=0;
	shaping_DCF=1;  //��ˮ�����л���ŷ�
	sonic_DCF  =0;	//����ǹ�л���ŷ�
	LED_B=1;LED_G=1;	//��ҳ�������ҳ��ر�S1��S2ˮƿ��	
	cutP_work_stop(3+cut_buff);  //����������ͣ��Ǹ�λ
	delayus(1000);	
	//cutP_S1_S2(3); //����S1,S2ˮƿ��λ
	//delayus(1000);	
	cutP_wate_HL(3+cut_buff);  //����ˮλ������λ
	delayus(1000);	
	Write_date(0x00,0x40,0x30,0);
	Write_date(0x00,0x41,0x00,30);
	Write_date(0x00,0x50,0x00,0x00);

	while(1)
	{  // 

		if(deal_tm_bit)
			deal_tm();
		if(time_out){time_out=0;minute=30;second=0;on_off=0;RF=0;cooling=0;}
		if(FS_ONE) 
		{ cutP_wate_HL(11+cut_buff); 
		S2BUF=0xe0;FS_ONE=0;
		if(CE_buffer==1){ cutP_work_stop(29);} //����������ͣ
		else            { cutP_work_stop(27);} //����Ӣ����ͣ 
		}
		if(beep_sw){ BEEP_wate();} 
		else{ cutP_wate_HL(3+cut_buff); }     //���ˮλ����
		if(key_bit){
			key_bit=0;
			key=(Get_Date[3]<<8)+(int)Get_Date[2]&0x00ff;
			switch(key){
			case 0x00f0: if(!beep_sw){ on_off=Get_Date[0]&0x0f;}
						 if((on_off)&&(!wate_sw)){beep_sw=0;FS_ONE=0;cutP_wate_HL(3);S2BUF=0xe1;}
						 else{ S2BUF=0xe0;} 									 
						 if(on_off){ pump=0;if(CE_buffer==1){ cutP_work_stop(28);}else{ cutP_work_stop(26);}}
						 else      { pump=1;if(CE_buffer==1){ cutP_work_stop(29);}else{ cutP_work_stop(27);}}
						 break;	 //����0XE0������
			case 0x0041:minute=Get_Date[0];dis_time();break;
			case 0x0010:s1_s2=1;cutP_S1_S2(3);S2BUF=0x20;break; 
			case 0x0020:s1_s2=0;cutP_S1_S2(11);S2BUF=0x21;break; 						 
			case 0x00a0:S2BUF=0xa0;delayus(1000);on_off=0;RF=0;cooling=0;
				wate_time=0;beep_sw=0;
				s1_s2=1;pump=1;shaping_DCF=1;sonic_DCF=1;Page=0x30;return;break;
			}

		}

	}
}	
//==================================================================================================================================
/*void Oxygun_picture_02()	  //07ҳ��  ����ǹ  ���л���ŷ�
{
int key=0;
minute=30;second=0;
on_off=0;RF=0;cooling=0;
shaping_DCF=1;  //��ˮ�����л���ŷ�
sonic_DCF  =0;	//����ǹ�л���ŷ�
cutP_work_stop(3+cut_buff);	
delayus(1000);	
cutP_wate_HL(3+cut_buff);  //����ˮλ������λ	
Write_date(0x00,0x40,0x30,0);
Write_date(0x00,0x41,0x00,30);
Write_date(0x00,0x50,0x00,0x00);

while(1)
{  // 

if(deal_tm_bit)
deal_tm();
if(time_out){time_out=0;minute=30;second=0;on_off=0;RF=0;cooling=0;}

if(key_bit){
key_bit=0;
key=(Get_Date[3]<<8)+(int)Get_Date[2]&0x00ff;

switch(key){
case 0x00f0: on_off=Get_Date[0]&0x0f; S2BUF=0xe0+on_off;
if(on_off){ pump=0;if(CE_buffer==1){ cutP_work_stop(28);}else{ cutP_work_stop(26);}}
else      { pump=1;if(CE_buffer==1){ cutP_work_stop(29);}else{ cutP_work_stop(27);}}
break;	 //����0XE0������
case 0x0041:minute=Get_Date[0];dis_time();BEEP();break;
case 0x00a0:S2BUF=0xa0;delayus(1000);on_off=0;RF=0;cooling=0;pump=1;shaping_DCF=1;sonic_DCF=1;Page=0x30;return;break;
}

}

}

}*/
//============================================
void FaceRF_picture()	   //02ҳ�� �沿RF
{

	int key=0;
	minute=30;second=0;
	on_off=0;RF=0;cooling=0;
	shaping_DCF=1;  //��ˮ�����л���ŷ�
	sonic_DCF  =1;	//����ǹ�л���ŷ�
	LED_B=1;LED_G=1;	//��ҳ�������ҳ��ر�S1��S2ˮƿ��	
	cutP_work_stop(3+cut_buff);		
	Write_date(0x00,0x40,0x30,0);
	Write_date(0x00,0x41,0x00,30);
	Write_date(0x00,0x50,0x00,0x00);
	Write_date(0x00,0x60,0x00,0x00);
	while(1)
	{  // 

		if(deal_tm_bit)
			deal_tm();
		if(time_out){time_out=0;minute=30;second=0;on_off=0;RF=0;vac_mode=0;}

		if(key_bit){
			key_bit=0;
			key=(Get_Date[3]<<8)+(int)Get_Date[2]&0x00ff;

			switch(key){
			case 0x00f0: on_off=Get_Date[0]&0x0f;S2BUF=0xf0+on_off;
				if(on_off){ if(CE_buffer==1){ cutP_work_stop(28);}else{ cutP_work_stop(26);}}
				else      { if(CE_buffer==1){ cutP_work_stop(29);}else{ cutP_work_stop(27);}}	 
				break;
			case 0x0050:RF=Get_Date[0];S2BUF=0x10+RF;break;	  //����0x10|ǿ�ȸ��°�
			case 0x0041:minute=Get_Date[0];dis_time();break;
			case 0x00a0:S2BUF=0xa0;delayus(1000);on_off=0;RF=0;cooling=0;BEEP();Page=0x30;return;break;
			}

		}

	}

}
//====================================================================================================
void Cooling_picture()	//03 ��
{
	int key=0;
	minute=30;second=0;
	on_off=0;RF=0;cooling=0;
	shaping_DCF=1;  //��ˮ�����л���ŷ�
	sonic_DCF  =1;	//����ǹ�л���ŷ�
	LED_B=1;LED_G=1;	//��ҳ�������ҳ��ر�S1��S2ˮƿ��	
	cutP_work_stop(3+cut_buff);		
	Write_date(0x00,0x40,0x30,0);
	Write_date(0x00,0x41,0x00,30);
	Write_date(0x00,0x50,0x00,0x00);
	Write_date(0x00,0x60,0x00,0x00);

	while(1)
	{  // 

		if(deal_tm_bit)
			deal_tm();
		if(time_out){time_out=0;minute=30;second=0;on_off=0;RF=0;vac_mode=0;}

		if(key_bit){
			key_bit=0;
			key=(Get_Date[3]<<8)+(int)Get_Date[2]&0x00ff;

			switch(key){
				//	case 0x0000:Page=Get_Date[0];return;break;
			case 0x00f0: on_off=Get_Date[0]&0x0f;S2BUF=0xf0+on_off;
				if(on_off){ if(CE_buffer==1){ cutP_work_stop(28);}else{ cutP_work_stop(26);}}
				else      { if(CE_buffer==1){ cutP_work_stop(29);}else{ cutP_work_stop(27);}}				 
				break;
			case 0x0041:minute=Get_Date[0];dis_time();break;
			case 0x0050:cooling=Get_Date[0];S2BUF=0x60|cooling;break;	//����0x60|ǿ�ȸ��°�
			case 0x00a0:S2BUF=0xa0;delayus(1000);cooling=0;on_off=0;RF=0;cooling=0;Page=0x30;return;break;
			}

		}

	}

}
//====================================================================================================
void check_picture()	 //04ҳ��   ����ͷ ���
{

	int key=0;
	minute=30;second=0;
	on_off=0;RF=0;vac_mode=0;
	LED_B=1;LED_G=1;	//��ҳ�������ҳ��ر�S1��S2ˮƿ��	
	S2BUF=0xC1;
	GoToVideo();
	while(1)
	{  // 
		if(deal_tm_bit)
			deal_tm();
		if(time_out){time_out=0;minute=30;second=0;on_off=0;RF=0;vac_mode=0;}
		if(key_bit){
			key_bit=0;
			key=(Get_Date[3]<<8)+(int)Get_Date[2]&0x00ff;
			switch(key){
			case 0x00b0:ExitVideo();S2BUF=0xC2;delayus(1000);on_off=0;RF=0;cooling=0;Page=0x30;return;break;
			}

		}

	}
} 
//===============================================================================================================================
void bio222_picture()   //05  ΢��
{

	int key=0;
	minute=30;second=0;
	on_off=0;RF=0;
	shaping_DCF=1;  //��ˮ�����л���ŷ�
	sonic_DCF  =1;	//����ǹ�л���ŷ�
	LED_B=1;LED_G=1;	//��ҳ�������ҳ��ر�S1��S2ˮƿ��	
	cutP_work_stop(3+cut_buff);		
	Write_date(0x00,0x40,0x30,0);
	Write_date(0x00,0x41,0x00,30);
	Write_date(0x00,0x50,0x00,0x00);
	Write_date(0x00,0x60,0x00,0x00);
	Write_date(0x00,0x70,0x00,0x00);
	while(1)
	{  // 
		if(deal_tm_bit)
			deal_tm();
		if(time_out){time_out=0;minute=30;second=0;on_off=0;RF=0;vac_mode=0;}
		if(key_bit){
			key_bit=0;
			key=(Get_Date[3]<<8)+(int)Get_Date[2]&0x00ff;
			switch(key){
			case 0x00f0:on_off=Get_Date[0]&0x0f;S2BUF=0xf0+on_off;
				if(on_off){ if(CE_buffer==1){ cutP_work_stop(28);}else{ cutP_work_stop(26);}}
				else      { if(CE_buffer==1){ cutP_work_stop(29);}else{ cutP_work_stop(27);}}										 
				break;
			case 0x0060:vac_mode=Get_Date[0];S2BUF=0x90+vac_mode;break;
			case 0x0050:RF=Get_Date[0];S2BUF=0x80+RF;break;
			case 0x0041:minute=Get_Date[0];dis_time();break;
			case 0x00a0:S2BUF=0xa0;delayus(1000);on_off=0;RF=0;vac_mode=0;Page=0x30;return;break;
			}
		}

	}

}
//============================================================================================================================
void bio333_picture()   //06 ΢�羭��
{
	int key=0;
	minute=30;second=0;
	on_off=0;RF=0;
	shaping_DCF=1;  //��ˮ�����л���ŷ�
	sonic_DCF  =1;	//����ǹ�л���ŷ�
	LED_B=1;LED_G=1;	//��ҳ�������ҳ��ر�S1��S2ˮƿ��	
	cutP_work_stop(3+cut_buff);		
	Write_date(0x00,0x40,0x30,0);
	Write_date(0x00,0x41,0x00,30);
	Write_date(0x00,0x50,0x00,0x00);
	Write_date(0x00,0x60,0x00,0x00);
	Write_date(0x00,0x70,0x00,0x00);
	while(1)
	{  // 
		if(deal_tm_bit)
			deal_tm();
		if(time_out)
		{
			time_out=0;
			minute=30;
			second=0;
			on_off=0;
			RF=0;
			vac_mode=0;
		}
		if(key_bit)
		{
			key_bit=0;
			key=(Get_Date[3]<<8)+(int)Get_Date[2]&0x00ff;
			switch(key)
			{
			case 0x00f0:
				{
					on_off=Get_Date[0]&0x0f;S2BUF=0xf0+on_off;
					if(on_off)
					{ 
						if(CE_buffer==1)
						{ cutP_work_stop(28);}
						else
						{ cutP_work_stop(26);}
					}
					else      
					{
						if(CE_buffer==1)
						{ cutP_work_stop(29);}
						else
						{ cutP_work_stop(27);}
					}										 
					break;
				}
			case 0x0060:
				{
					vac_mode=Get_Date[0];
					S2BUF=0x90+vac_mode;
					break;
				}
			case 0x0050:
				{
					RF=Get_Date[0];
					S2BUF=0x70+RF;
					break;
				}				
			case 0x0041:
				{
					minute=Get_Date[0];
					dis_time();
					break;
				}
			case 0x00a0:
				{
					S2BUF=0xa0;
					delayus(1000);
					on_off=0;RF=0;
					vac_mode=0;
					Page=0x30;
					return;
					break;
				}
			}

		}

	}

}
//==================================================================================================================================
/*void eyes_picture()		//05ҳ��  �۲���Ƶ  
{

int key=0;
minute=30;second=0;
on_off=0;RF=0;cooling=0;
shaping_DCF=1;  //��ˮ�����л���ŷ�
sonic_DCF  =1;	//����ǹ�л���ŷ�
cutP_work_stop(3+cut_buff);			
Write_date(0x00,0x40,0x30,0);
Write_date(0x00,0x41,0x00,30);
Write_date(0x00,0x50,0x00,0x00);
Write_date(0x00,0x60,0x00,0x00);

while(1)
{  // 

if(deal_tm_bit)
deal_tm();
if(time_out){time_out=0;minute=30;second=0;on_off=0;RF=0;vac_mode=0;}

if(key_bit){
key_bit=0;
key=(Get_Date[3]<<8)+(int)Get_Date[2]&0x00ff;

switch(key){

//	case 0x0000:Page=Get_Date[0];return;break;
case 0x00f0: on_off=Get_Date[0]&0x0f;S2BUF=0xf0+on_off;
if(on_off){ if(CE_buffer==1){ cutP_work_stop(28);}else{ cutP_work_stop(26);}}
else      { if(CE_buffer==1){ cutP_work_stop(29);}else{ cutP_work_stop(27);}}				 
break;
case 0x0050:RF=Get_Date[0];S2BUF=0x50+RF;break;	  //����0x50|ǿ�ȸ��°�
case 0x0041:minute=Get_Date[0];dis_time();break;
case 0x00a0:S2BUF=0xa0;delayus(1000);on_off=0;RF=0;cooling=0;Page=0x30;return;break;
}

}

}

}
void bio_picture()  //04ҳ��   BIO����ͷ
{

int key=0;
minute=30;second=0;
on_off=0;RF=0;vac_mode=0;
shaping_DCF=1;  //��ˮ�����л���ŷ�
sonic_DCF  =1;	//����ǹ�л���ŷ�
cutP_work_stop(2);		
Write_date(0x00,0x40,0x30,0);
Write_date(0x00,0x41,0x00,30);
Write_date(0x00,0x50,0x00,0x00);
Write_date(0x00,0x60,0x00,0x00);
Write_date(0x00,0x70,0x00,0x00);
while(1)
{  // 

if(deal_tm_bit)
deal_tm();
if(time_out){time_out=0;minute=30;second=0;on_off=0;RF=0;vac_mode=0;}

if(key_bit){
key_bit=0;
key=(Get_Date[3]<<8)+(int)Get_Date[2]&0x00ff;

switch(key){
case 0x00f0: on_off=Get_Date[0]&0x0f;S2BUF=0xf0+on_off;
if(on_off){ if(CE_buffer==1){ cutP_work_stop(28);}else{ cutP_work_stop(26);}}
else      { if(CE_buffer==1){ cutP_work_stop(29);}else{ cutP_work_stop(27);}}					 
break;
case 0x0060:vac_mode=Get_Date[0];S2BUF=0x90+vac_mode;break;
case 0x00b0:on_off=0;S2BUF=0xc0;cutP_work_stop(11);vac_mode=0;RF=0;GoToVideo();break;
case 0x00c0:ExitVideo();S2BUF=0xa0;on_off=0;Write_date(0x00,0x50,0x00,0x00);Write_date(0x00,0x60,0x00,0x00);
cutP_work_stop(11);RF=0;break;
case 0x0050:RF=Get_Date[0];S2BUF=0x70+RF;break;
case 0x0041:minute=Get_Date[0];dis_time();break;
case 0x00a0:S2BUF=0xa0;delayus(1000);on_off=0;RF=0;vac_mode=0;Page=0x30;return;break;
}

}

}

}*/
//=================================================================================================
void main(void)
{
	//  unsigned char k=0,i=0,j=0;
	CPU_Init();
	P4SW=0x70;
	P4M0=0xFF;
	P4M1=0;
	P0M0=0xff;
	P0M1=0;
	LED_B=1;
	LED_G=1;	//��ҳ�������ҳ��ر�S1��S2ˮƿ��	
	BEEP();
	beep=0;
	delayus(10000);
	beep=0;
	delayus(10000);
	beep=0;
	delayus(10000);  //
	while(1)
	{
		switch(Page)
		{
			case 0x00:OPEN_picture();break;	     //����logo��
			case 0x30:main_picture();break;	     //��ҳ��  
			case 0x31:Aqua_picture();break;	     //01ˮ����ҳ��
			case 0x32:FaceRF_picture();break;	   //02�沿RF
			case 0x33:Cooling_picture();break;	   //03��
			case 0x34:check_picture();break;	     //04����ͷ
			case 0x35:bio222_picture();break;     //05΢��
			case 0x36:bio333_picture();break;     //06����
			case 0x37:Oxygun_picture();break;     //07����ǹ
		}
	}
}

void com_interrupt(void) interrupt 4 using 3
{
	if(RI)                      //���������ж�
	{
		RI=0;                    //����жϱ�־λ  
		if(DD==0)
		{
			if(xx<3)
			{
				xx++;
				pp[xx]=SBUF;	
				if(pp[1]!=0x5A)xx=0; 
			}
			else 
			{	
				W_OR_R=pp[4]=SBUF;
				if(pp[1]==0x5A)
				{ 
					DD=pp[3]-1; //W_OR_R=Buffer[4];
					pp=pp+5;
				}
				else{ xx=0;}	
			}
		}
		else
		{
			DD--;
			*pp++=SBUF;
			if(DD==0)
			{
				xx=0;
				key_bit=1;
				Get_Date[0]=*--pp;
				Get_Date[1]=*--pp;	
				pp--;
				Get_Date[2]=*--pp;
				Get_Date[3]=*--pp;
				pp=Buffer;//SBUF=0x88;
				beep=0;
			} //0���ֽڣ�1���ֽ�

		}
	}
	if(TI)                      //���������ж�
	{
		TI=0;                    //����жϱ�־λ   
		TT_bit=1;
	}
}

void time1 (void) interrupt 3
{

}

void Uart2 () interrupt 8 using 1
{
	if(S2CON&0x01)
	{
		S2CON&=~0x01;
	}
	if(S2CON&0x02)
	{
		S2CON&=~0x02;
	}
}

void time0 (void) interrupt 1
{//	 unsigned char xx;
	//static unsigned char cl=0;
	TF0=0;
	TH0 =(65536-1000)/256;
	TL0 =(65536-1000)%256;
	BEEP();
	if(on_off)
	{
		if(++time_count>2000)
		{
			time_count=0;
			deal_tm_bit=1;
		}
	}
	if((wate_sw)&&((Page==0x31)||(Page==0x37)))  //ˮλ��⣬�͵�ƽ��Ч
	{ 
		if(++wate_time==1000)
		{
			if(wate_sw)
			{
				wate_time=1100;
				on_off =0;
				beep_sw=1;
				FS_ONE=1;
			}  //ˮλ��⣬�͵�ƽ��Ч 
			else
			{
				wate_time=0;
				beep_sw=0;
			}
		}	 
	}	
	else
	{
		wate_time=0;
		beep_sw=0;
	}	
}