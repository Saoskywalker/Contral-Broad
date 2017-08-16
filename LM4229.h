/*---------------------------------------------------------------------
                         LCD128*240.H�ļ�
  ��    ��:�ٳ�Ⱥ     
  ��������:2009��10��8��16:58:45    
  ����޸�:2009��10��13��19:56:26
  ��    ����1.0
----------------------------------------------------------------------*/

#include "absacc.h"
#include "math.h"
#define uchar unsigned char
#define uint  unsigned int
#define DATA_PORT P0
//-------------------------------------------
//              �ܽŶ���   
//------------------------------------------

sbit CD = P2^6;     //����(L)/����(H)ѡ��
sbit CE = P2^5;     //ʹ��,�͵�ƽ��Ч
sbit R = P2^4;      //�����͵�ƽ��Ч
sbit W = P2^3;      //д���͵�ƽ��Ч
sbit Light=P2^2;
sbit beep=P2^1;
//sbit MD2 = P2^4;    //��ʽѡ��
//sbit FS1 = P2^5;    //����ģʽѡ��
//sbit HALT = P2^6;
//sbit RST = P2^7;    //��λ

//===========================================
//              ��������
//-------------------------------------------
void WriteByte(uchar);
void WriteCommand(uchar);
void LcdClr(uchar,uchar,uint);
void ShowDot(unsigned char,unsigned char,unsigned char);
void ShowPicture(uchar x,uchar y,uchar a,uchar b,uchar * add);
void LcdPrintBig(uchar y,uchar x,char *cha);
void fandisplay(uchar y,uchar x,char *cha);
int ReadByte();
void ClrWave();

/*********************************************************************************/
/*                               ����Ϊָ���                                  */
/*-------------------------------------------------------------------------------*/

//--------------------------------------------
//              ��ʱ����
//--------------------------------------------
void Delay(uchar t)
{
 for(;t!=0; t--);
}

//--------------------------------------------
//              д���ݺ���
//--------------------------------------------
void WriteByte(uchar dat)
{
    CD=0;          //����   
    CE=0;          //LCDʹ��
    W=0;        //д
    DATA_PORT=dat; //�������͵�P��
    Delay(2);      //������ϸ�ʱ����ʱ
    W=1;        //��д
    CE=1;          //��ʹ��
}

int ReadByte()
{
    CD=0;          //����   
    CE=0;          //LCDʹ��
	DATA_PORT=0xff;
    W=1;        //д
    DATA_PORT; //�������͵�P��
    Delay(2);      //������ϸ�ʱ����ʱ
    CE=1;          //��ʹ��
	return DATA_PORT; //�������͵�P��
}


//--------------------------------------------
//              д�����
//--------------------------------------------
void WriteCommand(uchar com)
{
    CD=1;    //����
    CE=0;
    W=0;
    DATA_PORT=com;
    Delay(2);    //������ϸ�ʱ����ʱ
    W=1;
    CE=1;
}

/*********************************************************************************/
/*                             ����Ϊ���ܺ���                                    */

//********************************************************************************
//��������LcdPrint
//��  ����uchar x,uchar y,char *cha
//��  �壺��������У�0~127����ˮƽ���ֽ�����0~127�����ַ���
//��  �ܣ�ָ��λ����LCDд��ָ���ַ�
//--------------------------------------------------------------------------------
void LcdPrint(uchar y,uchar x,char *cha)
{
    uint addr,i=0; //16λ��ַ
    uchar length,c1,c2,j,k;
    for(length=0;cha[length]!='\0';length++);  //��ȡ�����ַ���ֱ������
    do
    {
    c1=cha[i];      //ȡ�ַ�����ASCII��
    c2=cha[i+1];
    addr=x*30+y;      //x����ҳ��һҳ���ǵ�һ��һ�У���30�ֽڣ�240��
    if(c1<=128)        //С��128Ϊ�ַ�������128Ϊ����
    {
        for(k=0;k<16;++k)     //�ַ�����Ϊ16*8
        {
             WriteByte(addr&0xff);   //д���ַ��λ
             WriteByte(addr>>8);    //д���ַ��λ
             WriteCommand(0x24);      //��ַ���óɹ�
                                      //�������������RAM�еĵ�ַ�趨
             WriteCommand(0xb0);      /*------���������Զ�д-------*/
	                                  //ָ��ִ�к� MPU ���������ض�д��ʾ������RAM�����ݣ�
	                                  //ÿ��д���ַָ���Զ��� 1
             WriteByte(ASC_MSK[(c1-0x20)*16+k]); 
	                                    //���磺0��ASCII��Ϊ0x30,
                                      //��ASC_MSK�е�λ��Ϊ0x10*16,16�ֽ���������д��
             WriteCommand(0xb2);      //�Զ���λ���κε��Զ���д��Ҫ�Դ˽���
             addr=addr+30;          /*----��֤ÿһ���ֽڵ�д���Ҫ������һ�У�
                                      ------�Խ�16�е����ݻ�ԭ-----------------*/
         }
         if(y>=29)            //y>28ʱ����Ҫ����
         {
            x=x+16;          //�����õĸ߶���16�㣬���Ի���Ҫ��16С��
            y=0;
          }
         else                //���򣬲����У�����д��һ���ַ�
         {
            y++;
         }
         i++;
     }
    else                   //#####c1����128Ϊ����
    {
        for(j=0;j<sizeof(GB_16)/sizeof(GB_16[0]);j++)  //�Һ��֣�sizeof�������÷����Ժܿ�ȷ���ֿ�����ĳ���
        {
            if(c1==GB_16[j].Index[0]&&c2==GB_16[j].Index[1]) //�ð�������������~~
            break;            //�����ҵ�������   
         }
        if(j<sizeof(GB_16)/sizeof(GB_16[0]))   //------����������������
        {
            for(k=0;k<16;k++)     //�ַ�����Ϊ16*8
            {                     
                WriteByte(addr&0xff);   //д���ַ��λ
                WriteByte(addr>>8);    //д���ַ��λ
                WriteCommand(0x24);          //��ַ����
                WriteCommand(0xb0);            /*------���������Զ�д-------*/
                WriteByte(GB_16[j].Msk[k*2]);
			    WriteByte(GB_16[j].Msk[k*2+1]); //16*16,ÿ��2�ֽ�                              
                                                //��β����Զ�д�˸�����д���κ��ַָ���Զ��� 2
                WriteCommand(0xb2);             //�Զ���λ
                addr=addr+30;              //��һ��
            }
        }
        else             //-----û�ҵ���Ӧ������Ϳ��
        {
            for(k=0;k<16;k++)     //�ַ�����Ϊ16*8
            {
                WriteByte(addr&0xff);   //д���ַ��λ
                WriteByte(addr>>8);    //д���ַ��λ
                WriteCommand(0x24);          //��ַ����
                WriteCommand(0xb0);         
                WriteByte(0xff);              //д��
		   	    WriteByte(0xff); 
                WriteCommand(0xb2);          //�Զ���λ
                addr=addr+30;              //��һ��
            }
        }
        if(y>27)      //-------����
        {
            x+=16;
            y=0;
        }
        else          //--------���û��У�����һ����
        {
            y+=2;
        }
         i+=2;
      } 
    }
   while(i<length);
}
void LcdPrintBig(uchar y,uchar x,char *cha)
{
    uint addr,i=0; //16λ��ַ
	uchar length=0,gg;
	uint c1,q,k,p;
          //ȡ�ַ�����ASCII��
		  gg=x;
	 for(length=0;cha[length];length++,i++,y=y+3)  //��ȡ�����ַ���ֱ������
		for(p=0,k=0,x=gg,addr=x*30+y,q=3;p<48;p++,q=k+3,x++,addr=x*30+y)
       
        {
             WriteByte(addr&0xff);   //д���ַ��λ
             WriteByte(addr>>8);    //д���ַ��λ
             WriteCommand(0x24);      //��ַ���óɹ�
                                      //�������������RAM�еĵ�ַ�趨
            
	                                  //ָ��ִ�к� MPU ���������ض�д��ʾ������RAM�����ݣ�
	                                  //ÿ��д���ַָ���Զ��� 1


			 	  WriteCommand(0xb0);      /*------���������Զ�д-------*/
				for(;k<q;++k)     //�ַ�����Ϊ16*8		
             WriteByte(nAsciiDot[(cha[i]-0x20)*144+k]); 
	                                    //���磺0��ASCII��Ϊ0x30,
                                      //��ASC_MSK�е�λ��Ϊ0x10*16,16�ֽ���������д��
             WriteCommand(0xb2);      //�Զ���λ���κε��Զ���д��Ҫ�Դ˽���
			 
                       /*----��֤ÿһ���ֽڵ�д���Ҫ������һ�У�
                                      ------�Խ�16�е����ݻ�ԭ-----------------*/
		
         }
     //x����ҳ��һҳ���ǵ�һ��һ�У���30�ֽڣ�240��
}					   
//********************************************************************************
//��������ShowDot
//��  ����unsigned char X,unsigned char Y,unsigned char Mode
//��  �壺��������У�0~127����ˮƽ���ֽ�����0~127������ʾ��ʽ��0~1��
//��  �ܣ�ͼ�κ���,������㻭ͼ
//      ��ָ��λ����ʾһ����
//      �����ʾ��������256*256,��320�ģ�����Ҫ�޸�������� PointX,PointY������
//      Mode 1:��ʾ 0:����õ�
//--------------------------------------------------------------------------------
void ShowDot(unsigned char X,unsigned char Y,unsigned char Mode)
{
   unsigned int addr=0;
   unsigned char dat,n;                 
   addr=Y*30 + X/8;
   if(Mode)   n=0xf8; 
   else       n=0xf0; 
   dat=n|((~(X%8))&0x07);
   WriteByte(addr&0xff); 
   WriteByte(addr>>8);    
   WriteCommand(0x24);//���øõ����ڵ�Ԫ��ַ
  
   WriteCommand(dat); //???Ϊʲô��������
}

//********************************************************************************
//�� �� ����DrawLine
//��    ����unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2
//��    Χ����0~239������0~127������0~239������0~127��
//��    �ܣ��ӵ�һ�㵽�ڶ���Ļ���
//�㷨˼�룺Bresenham�㷨����ֱ��
//          �����������ߵı�����ϵ��ʵ�ֵġ�
//          ÿ��ѭ�������x��y��λ�ƴ������仯һ����λ��
//          ����ѭ�����������߱�����ϵʱ��x��y��λ��С������仯һ����λ��
//          Ϊ��ʹ���ߺ������ڰ˸������ܻ���������Ϊ���Ķ�ת������������
//--------------------------------------------------------------------------------
//void DrawLine(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
//{
//   int dx,dy,s1,s2,temp,interchange=0,p,i;
//   float x,y;
//   dx=abs(x2-x1);   //abs ����ֵ
//   dy=abs(y2-y1);
//   s1=(x2-x1)/dx;  //s1��s2��sign ���ţ�Ϊ�˾�������
//   s2=(y2-y1)/dy;
//   x=x1;y=y1; //��1 ��Ϊ���
//   if(dy>dx) //������I�������У��任����ϵ
//   {/*����б��mֵ*/
//	    temp=dx;
//	    dx=dy;
//        dy=temp;/*��֤dxΪ������ıߣ���㷽������I��������*/
//        interchange=1;
//   }
//   p=2*dy-dx; //I���������
//   for(i=1;i<=dx;i++)
//   {
//        ShowDot((int)x,(int)y,1);
//        if(p>0)
//        {
//            if(interchange)
//                x=x+s1;	     
//            else/*�����ǻ���*/
//                y=y+s2;
//
//            p=p-(dx<<1);
//        }
//         if(interchange) //���2*dy-dx<=0��
//
//            y=y+s2;     /*��yi����xi*/
//         else/*�����ǻ���*/
//            x=x+s1;
//
//         p=p+2*dy;
//    }
//} 
void liney(unsigned int x1,int y1,int y2)
{
	int x,y;
	if(y1>y2)
	for(x=0;x<y1-y2;x++)
  ShowDot(x1,y2+x,1);
    else
		for(x=0;x<y2-y1;x++)
  ShowDot(x1,y1+x,1);

}
void DrawLine(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{	int x,y,z;
	int a=0,b=0;
	float dt;
	if(x2>239)x2=239;
	if(y1>y2)
	{
	 dt=(float)(y1-y2)/(x2-x1);
	 for(x=0;x<x2-x1+1;x++)
	 {	
	 a=b;
	 b=dt*x;
	 if(b-a>1)
	 liney(x1+x,y1-a,y1-b);
	 else
	 ShowDot(x1+x,y1-b,1);
	 }
	}
	else
	{
	 dt=(float)(y2-y1)/(x2-x1);
	 for(x=0;x<x2-x1+1;x++)
	 {
	 	 a=b;
	 b=dt*x;
	 if(b-a>1)
	 liney(x1+x,y1+b,y1+a+1);
	 else
	 ShowDot(x1+x,y1+b,1);
	 }
	}
} 
void fandisplay(uchar y,uchar x,char *cha)
{
    uint addr,i=0; //16λ��ַ
    uchar length,c1,c2,j,k;
    for(length=0;cha[length]!='\0';length++);  //��ȡ�����ַ���ֱ������
    do
    {
    c1=cha[i];      //ȡ�ַ�����ASCII��
    c2=cha[i+1];
    addr=x*30+y;      //x����ҳ��һҳ���ǵ�һ��һ�У���30�ֽڣ�240��
    if(c1<=128)        //С��128Ϊ�ַ�������128Ϊ����
    {
        for(k=0;k<16;++k)     //�ַ�����Ϊ16*8
        {
             WriteByte(addr&0xff);   //д���ַ��λ
             WriteByte(addr>>8);    //д���ַ��λ
             WriteCommand(0x24);      //��ַ���óɹ�
                                      //�������������RAM�еĵ�ַ�趨
             WriteCommand(0xb0);      /*------���������Զ�д-------*/
	                                  //ָ��ִ�к� MPU ���������ض�д��ʾ������RAM�����ݣ�
	                                  //ÿ��д���ַָ���Զ��� 1
             WriteByte(~ASC_MSK[(c1-0x20)*16+k]); 
	                                    //���磺0��ASCII��Ϊ0x30,
                                      //��ASC_MSK�е�λ��Ϊ0x10*16,16�ֽ���������д��
             WriteCommand(0xb2);      //�Զ���λ���κε��Զ���д��Ҫ�Դ˽���
             addr=addr+30;          /*----��֤ÿһ���ֽڵ�д���Ҫ������һ�У�
                                      ------�Խ�16�е����ݻ�ԭ-----------------*/
         }
         if(y>=29)            //y>28ʱ����Ҫ����
         {
            x=x+16;          //�����õĸ߶���16�㣬���Ի���Ҫ��16С��
            y=0;
          }
         else                //���򣬲����У�����д��һ���ַ�
         {
            y++;
         }
         i++;
     }
    else                   //#####c1����128Ϊ����
    {
        for(j=0;j<sizeof(GB_16)/sizeof(GB_16[0]);j++)  //�Һ��֣�sizeof�������÷����Ժܿ�ȷ���ֿ�����ĳ���
        {
            if(c1==GB_16[j].Index[0]&&c2==GB_16[j].Index[1]) //�ð�������������~~
            break;            //�����ҵ�������   
         }
        if(j<sizeof(GB_16)/sizeof(GB_16[0]))   //------����������������
        {
            for(k=0;k<16;k++)     //�ַ�����Ϊ16*8
            {                     
                WriteByte(addr&0xff);   //д���ַ��λ
                WriteByte(addr>>8);    //д���ַ��λ
                WriteCommand(0x24);          //��ַ����
                WriteCommand(0xb0);            /*------���������Զ�д-------*/
                WriteByte(~GB_16[j].Msk[k*2]);
			    WriteByte(~GB_16[j].Msk[k*2+1]); //16*16,ÿ��2�ֽ�                              
                                                //��β����Զ�д�˸�����д���κ��ַָ���Զ��� 2
                WriteCommand(0xb2);             //�Զ���λ
                addr=addr+30;              //��һ��
            }
        }
        else             //-----û�ҵ���Ӧ������Ϳ��
        {
            for(k=0;k<16;k++)     //�ַ�����Ϊ16*8
            {
                WriteByte(addr&0xff);   //д���ַ��λ
                WriteByte(addr>>8);    //д���ַ��λ
                WriteCommand(0x24);          //��ַ����
                WriteCommand(0xb0);         
                WriteByte(0xff);              //д��
		   	    WriteByte(0xff); 
                WriteCommand(0xb2);          //�Զ���λ
                addr=addr+30;              //��һ��
            }
        }
        if(y>27)      //-------����
        {
            x+=16;
            y=0;
        }
        else          //--------���û��У�����һ����
        {
            y+=2;
        }
         i+=2;
      } 
    }
   while(i<length);
}
//********************************************************************************
//�� �� ����DrawCircle
//��    ����unsigned char x0,unsigned char y0,unsigned r
//��    Χ����0~239������0~127������0~239������0~127��
//��    �ܣ��ӵ�һ�㵽�ڶ���Ļ���
//�㷨˼�룺Bresenham�㷨��Բ
//--------------------------------------------------------------------------------
void DrawCircle(unsigned int x0,unsigned int y0,unsigned int r)
{
    unsigned int x,y;
    signed int p;  
    x=0;
    y=r;
    p=5-4*r;
    for(;x<=y;x++)
    {
        ShowDot(x0+x,y0+y,1); //A
        ShowDot(x0+y,y0-x,1); //B
        ShowDot(x0-x,y0-y,1); //C
        ShowDot(x0-y,y0+x,1); //D
        ShowDot(x0-x,y0+y,1); //E
        ShowDot(x0-y,y0-x,1); //F
        ShowDot(x0+x,y0-y,1); //G
        ShowDot(x0+y,y0+x,1); //H
        if(p>0)
        {
             p+=8-(y<<3);     /*�˾�һ��Ҫ����ǰ�棬��Ϊ�о���ʽ��Pi+2ʱ�Ľ����yi*/
             y--;              
        }
        p+=12+(x<<3);

        
    }       
}


//********************************************************************************
// ȫ����ʾһ��ͼƬ 128*240
//-------------------------------------------------------------------------------   
void Show128x240(uchar * add)
{
    uchar i=0,j=0, *address;
    address=add;
    WriteByte(0);   //д���ַ��λ
    WriteByte(0);    //д���ַ��λ
    WriteCommand(0x24);          //��ַ����
            
    for(j=0;j<128;j++)          
        for(i=0;i<30;i++)
    	{   
            WriteCommand(0xb0);         //�Զ������� 
            WriteByte(address[30*j+i]); 
            WriteCommand(0xb2);          //�Զ���λ
         }   
}

void ShowPicture(uchar x,uchar y,uchar a,uchar b,uchar * add)
{
    uchar i=0,j=0,k=0;
	uint addr;
	for(i=0;i<b;i++) 
	{     
    addr=(y+i)*30+x;      //x����ҳ��һҳ���ǵ�һ��һ�У���30�ֽڣ�240��
             WriteByte(addr&0xff);   //д���ַ��λ
             WriteByte(addr>>8);    //д���ַ��λ
             WriteCommand(0x24);      //��ַ���óɹ�
                                      //�������������RAM�еĵ�ַ�趨
             WriteCommand(0xb0);      /*------���������Զ�д-------*/
	                                  //ָ��ִ�к� MPU ���������ض�д��ʾ������RAM�����ݣ�
	                                  //ÿ��д���ַָ���Զ��� 1
		if(1)
		{
		for(j=0;j<a;j++) 
             WriteByte(*(add++)); 
 
		}                      //��ASC_MSK�е�λ��Ϊ0x10*16,16�ֽ���������д��
		else
		{
		  for(j=0;j<4;j++) 
             WriteByte(*(add++));  //���磺0��ASCII��Ϊ0x30,

			WriteCommand(0xb2);      //�Զ���λ���κε��Զ���д��Ҫ�Դ˽���                     //��ASC_MSK�е�λ��Ϊ0x10*16,16�ֽ���������д��
			     addr=(y+i)*30+3;      //x����ҳ��һҳ���ǵ�һ��һ�У���30�ֽڣ�240��
             WriteByte(addr&0xff);   //д���ַ��λ
             WriteByte(addr>>8);    //д���ַ��λ
             WriteCommand(0x24);      //��ַ���óɹ�
                                      //�������������RAM�еĵ�ַ�趨
			 WriteCommand(0xb0);
			 for(j=0;j<a-3;j++) 
             WriteByte(*(add++)); 

		  }
            // addr=addr+30;          /*----��֤ÿһ���ֽڵ�д���Ҫ������һ�У�
            //  
		WriteCommand(0xb2);                       // ------�Խ�16�е����ݻ�ԭ-----------------*/
         }  
}
//********************************************************************************
// 	ָ��λ�á��������� ,һ��һ���ַ�
//-------------------------------------------------------------------------------
void ClrChar(uchar x,uchar y,uint length)
{
uchar i;
for(i=0;i<length;i++) 
   {
   LcdPrint(x,y," "); 
   y=1+y;
   }
}

//********************************************************************************
//           ָ��һ��
//--------------------------------------------------------------------------------
void Clrline(uchar x)
{
   LcdPrint(x*16,0,"                              "); 
}

void ClrWave()
{  uchar x,y;
	uint addr;

//		for(y=12,x=20;y<30;y++)
//	{
//  	addr=x*30+y;      //x����ҳ��һҳ���ǵ�һ��һ�У���30�ֽڣ�240��
//	               WriteByte(addr&0xff);   //д���ַ��λ
//             WriteByte(addr>>8);    //д���ַ��λ
//             WriteCommand(0x24);      //��ַ���óɹ�
//              WriteCommand(0xb0);      /*------���������Զ�д-------*/                         //�������������RAM�еĵ�ַ�趨
//		for(;x<64;x++)
// 	 WriteCommand(0x00); 
//		 WriteCommand(0xb2); 
//	 }
	

		for(x=20,y=12;x<65;x++)
	{
	y=12;
  	addr=x*30+y;      //x����ҳ��һҳ���ǵ�һ��һ�У���30�ֽڣ�240��
	               WriteByte(addr&0xff);   //д���ַ��λ
             WriteByte(addr>>8);    //д���ַ��λ
             WriteCommand(0x24);      //��ַ���óɹ�
              WriteCommand(0xb0);      /*------���������Զ�д-------*/                         //�������������RAM�еĵ�ַ�趨
		for(y=12;y<29;y++)
 	 WriteByte(0x00); 

		 WriteCommand(0xb2); 
	 }

	 		for(x=67,y=12;x<110;x++)
	{
	y=12;
  	addr=x*30+y;      //x����ҳ��һҳ���ǵ�һ��һ�У���30�ֽڣ�240��
	               WriteByte(addr&0xff);   //д���ַ��λ
             WriteByte(addr>>8);    //д���ַ��λ
             WriteCommand(0x24);      //��ַ���óɹ�
              WriteCommand(0xb0);      /*------���������Զ�д-------*/                         //�������������RAM�еĵ�ַ�趨
		for(y=12;y<29;y++)
 	 WriteByte(0x00); 

		 WriteCommand(0xb2); 
	 }


}
//********************************************************************************
//           ��������
//--------------------------------------------------------------------------------
void  ClrScr()
{
    uchar i=0,j=0;
    WriteByte(0);   //д���ַ��λ
    WriteByte(0);    //д���ַ��λ
    WriteCommand(0x24);          //��ַ����
            
    for(j=0;j<128;j++)          
        for(i=0;i<30;i++)
    	{   
            WriteCommand(0xb0);         //�Զ������� 
            WriteByte(0x00);            //����
            WriteCommand(0xb2);          //�Զ���λ
         }
}
