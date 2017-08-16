/*---------------------------------------------------------------------
                         LCD128*240.H文件
  作    者:荣超群     
  创建日期:2009年10月8日16:58:45    
  最后修改:2009年10月13日19:56:26
  版    本：1.0
----------------------------------------------------------------------*/

#include "absacc.h"
#include "math.h"
#define uchar unsigned char
#define uint  unsigned int
#define DATA_PORT P0
//-------------------------------------------
//              管脚定义   
//------------------------------------------

sbit CD = P2^6;     //数据(L)/命令(H)选择
sbit CE = P2^5;     //使能,低电平有效
sbit R = P2^4;      //读，低电平有效
sbit W = P2^3;      //写，低电平有效
sbit Light=P2^2;
sbit beep=P2^1;
//sbit MD2 = P2^4;    //方式选择
//sbit FS1 = P2^5;    //字体模式选择
//sbit HALT = P2^6;
//sbit RST = P2^7;    //复位

//===========================================
//              函数声明
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
/*                               以下为指令函数                                  */
/*-------------------------------------------------------------------------------*/

//--------------------------------------------
//              延时函数
//--------------------------------------------
void Delay(uchar t)
{
 for(;t!=0; t--);
}

//--------------------------------------------
//              写数据函数
//--------------------------------------------
void WriteByte(uchar dat)
{
    CD=0;          //数据   
    CE=0;          //LCD使能
    W=0;        //写
    DATA_PORT=dat; //把数据送到P口
    Delay(2);      //当晶振较高时加延时
    W=1;        //关写
    CE=1;          //关使能
}

int ReadByte()
{
    CD=0;          //数据   
    CE=0;          //LCD使能
	DATA_PORT=0xff;
    W=1;        //写
    DATA_PORT; //把数据送到P口
    Delay(2);      //当晶振较高时加延时
    CE=1;          //关使能
	return DATA_PORT; //把数据送到P口
}


//--------------------------------------------
//              写命令函数
//--------------------------------------------
void WriteCommand(uchar com)
{
    CD=1;    //命令
    CE=0;
    W=0;
    DATA_PORT=com;
    Delay(2);    //当晶振较高时加延时
    W=1;
    CE=1;
}

/*********************************************************************************/
/*                             以下为功能函数                                    */

//********************************************************************************
//函数名：LcdPrint
//参  数：uchar x,uchar y,char *cha
//含  义：竖方向的行（0~127）；水平的字节数（0~127），字符串
//功  能：指定位置向LCD写入指定字符
//--------------------------------------------------------------------------------
void LcdPrint(uchar y,uchar x,char *cha)
{
    uint addr,i=0; //16位地址
    uchar length,c1,c2,j,k;
    for(length=0;cha[length]!='\0';length++);  //读取所有字符，直到结束
    do
    {
    c1=cha[i];      //取字符串的ASCII码
    c2=cha[i+1];
    addr=x*30+y;      //x还是页，一页就是单一的一行，共30字节，240点
    if(c1<=128)        //小于128为字符，大于128为汉字
    {
        for(k=0;k<16;++k)     //字符设置为16*8
        {
             WriteByte(addr&0xff);   //写入地址低位
             WriteByte(addr>>8);    //写入地址高位
             WriteCommand(0x24);      //地址设置成功
                                      //上面三句完成了RAM中的地址设定
             WriteCommand(0xb0);      /*------设置数据自动写-------*/
	                                  //指令执行后 MPU 可以连续地读写显示缓冲区RAM的数据，
	                                  //每读写后地址指针自动增 1
             WriteByte(ASC_MSK[(c1-0x20)*16+k]); 
	                                    //例如：0的ASCII码为0x30,
                                      //在ASC_MSK中的位置为0x10*16,16字节字码依次写入
             WriteCommand(0xb2);      //自动复位，任何的自动读写后都要以此结束
             addr=addr+30;          /*----保证每一个字节的写入后都要换到下一行，
                                      ------以将16行的数据还原-----------------*/
         }
         if(y>=29)            //y>28时候需要换行
         {
            x=x+16;          //都是用的高度是16点，所以换行要换16小行
            y=0;
          }
         else                //否则，不换行，继续写下一个字符
         {
            y++;
         }
         i++;
     }
    else                   //#####c1大于128为汉字
    {
        for(j=0;j<sizeof(GB_16)/sizeof(GB_16[0]);j++)  //找汉字，sizeof的这样用法可以很快确定字库数组的长度
        {
            if(c1==GB_16[j].Index[0]&&c2==GB_16[j].Index[1]) //好棒啊，带索引了~~
            break;            //汉字找到则跳出   
         }
        if(j<sizeof(GB_16)/sizeof(GB_16[0]))   //------如果汉字在子码表内
        {
            for(k=0;k<16;k++)     //字符设置为16*8
            {                     
                WriteByte(addr&0xff);   //写入地址高位
                WriteByte(addr>>8);    //写入地址低位
                WriteCommand(0x24);          //地址设置
                WriteCommand(0xb0);            /*------设置数据自动写-------*/
                WriteByte(GB_16[j].Msk[k*2]);
			    WriteByte(GB_16[j].Msk[k*2+1]); //16*16,每行2字节                              
                                                //这次不是自动写八个，读写两次后地址指针自动增 2
                WriteCommand(0xb2);             //自动复位
                addr=addr+30;              //下一行
            }
        }
        else             //-----没找到对应汉字则涂黑
        {
            for(k=0;k<16;k++)     //字符设置为16*8
            {
                WriteByte(addr&0xff);   //写入地址高位
                WriteByte(addr>>8);    //写入地址低位
                WriteCommand(0x24);          //地址设置
                WriteCommand(0xb0);         
                WriteByte(0xff);              //写黑
		   	    WriteByte(0xff); 
                WriteCommand(0xb2);          //自动复位
                addr=addr+30;              //下一行
            }
        }
        if(y>27)      //-------换行
        {
            x+=16;
            y=0;
        }
        else          //--------不用换行，则下一个字
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
    uint addr,i=0; //16位地址
	uchar length=0,gg;
	uint c1,q,k,p;
          //取字符串的ASCII码
		  gg=x;
	 for(length=0;cha[length];length++,i++,y=y+3)  //读取所有字符，直到结束
		for(p=0,k=0,x=gg,addr=x*30+y,q=3;p<48;p++,q=k+3,x++,addr=x*30+y)
       
        {
             WriteByte(addr&0xff);   //写入地址低位
             WriteByte(addr>>8);    //写入地址高位
             WriteCommand(0x24);      //地址设置成功
                                      //上面三句完成了RAM中的地址设定
            
	                                  //指令执行后 MPU 可以连续地读写显示缓冲区RAM的数据，
	                                  //每读写后地址指针自动增 1


			 	  WriteCommand(0xb0);      /*------设置数据自动写-------*/
				for(;k<q;++k)     //字符设置为16*8		
             WriteByte(nAsciiDot[(cha[i]-0x20)*144+k]); 
	                                    //例如：0的ASCII码为0x30,
                                      //在ASC_MSK中的位置为0x10*16,16字节字码依次写入
             WriteCommand(0xb2);      //自动复位，任何的自动读写后都要以此结束
			 
                       /*----保证每一个字节的写入后都要换到下一行，
                                      ------以将16行的数据还原-----------------*/
		
         }
     //x还是页，一页就是单一的一行，共30字节，240点
}					   
//********************************************************************************
//函数名：ShowDot
//参  数：unsigned char X,unsigned char Y,unsigned char Mode
//含  义：竖方向的行（0~127）；水平的字节数（0~127），显示方式（0~1）
//功  能：图形函数,用于描点画图
//      在指定位置显示一个点
//      如果显示屏超过了256*256,像320的，就需要修改这个函数 PointX,PointY的类型
//      Mode 1:显示 0:清除该点
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
   WriteCommand(0x24);//设置该点所在单元地址
  
   WriteCommand(dat); //???为什么？？？？
}

//********************************************************************************
//函 数 名：DrawLine
//参    数：unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2
//范    围：（0~239），（0~127），（0~239），（0~127）
//功    能：从第一点到第二点的画线
//算法思想：Bresenham算法绘制直线
//          过横线与竖线的倍数关系来实现的。
//          每次循环画点后x、y中位移大的坐标变化一个单位，
//          而当循环次数是两者倍数关系时，x、y中位移小的坐标变化一个单位。
//          为了使画线函数能在八个方向都能画，将增量为负的都转化正的增量。
//--------------------------------------------------------------------------------
//void DrawLine(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
//{
//   int dx,dy,s1,s2,temp,interchange=0,p,i;
//   float x,y;
//   dx=abs(x2-x1);   //abs 绝对值
//   dy=abs(y2-y1);
//   s1=(x2-x1)/dx;  //s1、s2是sign 符号，为了决定方向
//   s2=(y2-y1)/dy;
//   x=x1;y=y1; //以1 点为起点
//   if(dy>dx) //不是在I类象限中，变换坐标系
//   {/*决定斜率m值*/
//	    temp=dx;
//	    dx=dy;
//        dy=temp;/*保证dx为增长快的边，描点方程是在I类象限中*/
//        interchange=1;
//   }
//   p=2*dy-dx; //I类象限误差
//   for(i=1;i<=dx;i++)
//   {
//        ShowDot((int)x,(int)y,1);
//        if(p>0)
//        {
//            if(interchange)
//                x=x+s1;	     
//            else/*不考虑换限*/
//                y=y+s2;
//
//            p=p-(dx<<1);
//        }
//         if(interchange) //如果2*dy-dx<=0，
//
//            y=y+s2;     /*把yi当成xi*/
//         else/*不考虑换限*/
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
    uint addr,i=0; //16位地址
    uchar length,c1,c2,j,k;
    for(length=0;cha[length]!='\0';length++);  //读取所有字符，直到结束
    do
    {
    c1=cha[i];      //取字符串的ASCII码
    c2=cha[i+1];
    addr=x*30+y;      //x还是页，一页就是单一的一行，共30字节，240点
    if(c1<=128)        //小于128为字符，大于128为汉字
    {
        for(k=0;k<16;++k)     //字符设置为16*8
        {
             WriteByte(addr&0xff);   //写入地址低位
             WriteByte(addr>>8);    //写入地址高位
             WriteCommand(0x24);      //地址设置成功
                                      //上面三句完成了RAM中的地址设定
             WriteCommand(0xb0);      /*------设置数据自动写-------*/
	                                  //指令执行后 MPU 可以连续地读写显示缓冲区RAM的数据，
	                                  //每读写后地址指针自动增 1
             WriteByte(~ASC_MSK[(c1-0x20)*16+k]); 
	                                    //例如：0的ASCII码为0x30,
                                      //在ASC_MSK中的位置为0x10*16,16字节字码依次写入
             WriteCommand(0xb2);      //自动复位，任何的自动读写后都要以此结束
             addr=addr+30;          /*----保证每一个字节的写入后都要换到下一行，
                                      ------以将16行的数据还原-----------------*/
         }
         if(y>=29)            //y>28时候需要换行
         {
            x=x+16;          //都是用的高度是16点，所以换行要换16小行
            y=0;
          }
         else                //否则，不换行，继续写下一个字符
         {
            y++;
         }
         i++;
     }
    else                   //#####c1大于128为汉字
    {
        for(j=0;j<sizeof(GB_16)/sizeof(GB_16[0]);j++)  //找汉字，sizeof的这样用法可以很快确定字库数组的长度
        {
            if(c1==GB_16[j].Index[0]&&c2==GB_16[j].Index[1]) //好棒啊，带索引了~~
            break;            //汉字找到则跳出   
         }
        if(j<sizeof(GB_16)/sizeof(GB_16[0]))   //------如果汉字在子码表内
        {
            for(k=0;k<16;k++)     //字符设置为16*8
            {                     
                WriteByte(addr&0xff);   //写入地址高位
                WriteByte(addr>>8);    //写入地址低位
                WriteCommand(0x24);          //地址设置
                WriteCommand(0xb0);            /*------设置数据自动写-------*/
                WriteByte(~GB_16[j].Msk[k*2]);
			    WriteByte(~GB_16[j].Msk[k*2+1]); //16*16,每行2字节                              
                                                //这次不是自动写八个，读写两次后地址指针自动增 2
                WriteCommand(0xb2);             //自动复位
                addr=addr+30;              //下一行
            }
        }
        else             //-----没找到对应汉字则涂黑
        {
            for(k=0;k<16;k++)     //字符设置为16*8
            {
                WriteByte(addr&0xff);   //写入地址高位
                WriteByte(addr>>8);    //写入地址低位
                WriteCommand(0x24);          //地址设置
                WriteCommand(0xb0);         
                WriteByte(0xff);              //写黑
		   	    WriteByte(0xff); 
                WriteCommand(0xb2);          //自动复位
                addr=addr+30;              //下一行
            }
        }
        if(y>27)      //-------换行
        {
            x+=16;
            y=0;
        }
        else          //--------不用换行，则下一个字
        {
            y+=2;
        }
         i+=2;
      } 
    }
   while(i<length);
}
//********************************************************************************
//函 数 名：DrawCircle
//参    数：unsigned char x0,unsigned char y0,unsigned r
//范    围：（0~239），（0~127），（0~239），（0~127）
//功    能：从第一点到第二点的画线
//算法思想：Bresenham算法画圆
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
             p+=8-(y<<3);     /*此句一定要放在前面，因为判决公式推Pi+2时的结果的yi*/
             y--;              
        }
        p+=12+(x<<3);

        
    }       
}


//********************************************************************************
// 全屏显示一张图片 128*240
//-------------------------------------------------------------------------------   
void Show128x240(uchar * add)
{
    uchar i=0,j=0, *address;
    address=add;
    WriteByte(0);   //写入地址高位
    WriteByte(0);    //写入地址低位
    WriteCommand(0x24);          //地址设置
            
    for(j=0;j<128;j++)          
        for(i=0;i<30;i++)
    	{   
            WriteCommand(0xb0);         //自动读数据 
            WriteByte(address[30*j+i]); 
            WriteCommand(0xb2);          //自动复位
         }   
}

void ShowPicture(uchar x,uchar y,uchar a,uchar b,uchar * add)
{
    uchar i=0,j=0,k=0;
	uint addr;
	for(i=0;i<b;i++) 
	{     
    addr=(y+i)*30+x;      //x还是页，一页就是单一的一行，共30字节，240点
             WriteByte(addr&0xff);   //写入地址低位
             WriteByte(addr>>8);    //写入地址高位
             WriteCommand(0x24);      //地址设置成功
                                      //上面三句完成了RAM中的地址设定
             WriteCommand(0xb0);      /*------设置数据自动写-------*/
	                                  //指令执行后 MPU 可以连续地读写显示缓冲区RAM的数据，
	                                  //每读写后地址指针自动增 1
		if(1)
		{
		for(j=0;j<a;j++) 
             WriteByte(*(add++)); 
 
		}                      //在ASC_MSK中的位置为0x10*16,16字节字码依次写入
		else
		{
		  for(j=0;j<4;j++) 
             WriteByte(*(add++));  //例如：0的ASCII码为0x30,

			WriteCommand(0xb2);      //自动复位，任何的自动读写后都要以此结束                     //在ASC_MSK中的位置为0x10*16,16字节字码依次写入
			     addr=(y+i)*30+3;      //x还是页，一页就是单一的一行，共30字节，240点
             WriteByte(addr&0xff);   //写入地址低位
             WriteByte(addr>>8);    //写入地址高位
             WriteCommand(0x24);      //地址设置成功
                                      //上面三句完成了RAM中的地址设定
			 WriteCommand(0xb0);
			 for(j=0;j<a-3;j++) 
             WriteByte(*(add++)); 

		  }
            // addr=addr+30;          /*----保证每一个字节的写入后都要换到下一行，
            //  
		WriteCommand(0xb2);                       // ------以将16行的数据还原-----------------*/
         }  
}
//********************************************************************************
// 	指定位置、长度清屏 ,一次一个字符
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
//           指定一行
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
//  	addr=x*30+y;      //x还是页，一页就是单一的一行，共30字节，240点
//	               WriteByte(addr&0xff);   //写入地址低位
//             WriteByte(addr>>8);    //写入地址高位
//             WriteCommand(0x24);      //地址设置成功
//              WriteCommand(0xb0);      /*------设置数据自动写-------*/                         //上面三句完成了RAM中的地址设定
//		for(;x<64;x++)
// 	 WriteCommand(0x00); 
//		 WriteCommand(0xb2); 
//	 }
	

		for(x=20,y=12;x<65;x++)
	{
	y=12;
  	addr=x*30+y;      //x还是页，一页就是单一的一行，共30字节，240点
	               WriteByte(addr&0xff);   //写入地址低位
             WriteByte(addr>>8);    //写入地址高位
             WriteCommand(0x24);      //地址设置成功
              WriteCommand(0xb0);      /*------设置数据自动写-------*/                         //上面三句完成了RAM中的地址设定
		for(y=12;y<29;y++)
 	 WriteByte(0x00); 

		 WriteCommand(0xb2); 
	 }

	 		for(x=67,y=12;x<110;x++)
	{
	y=12;
  	addr=x*30+y;      //x还是页，一页就是单一的一行，共30字节，240点
	               WriteByte(addr&0xff);   //写入地址低位
             WriteByte(addr>>8);    //写入地址高位
             WriteCommand(0x24);      //地址设置成功
              WriteCommand(0xb0);      /*------设置数据自动写-------*/                         //上面三句完成了RAM中的地址设定
		for(y=12;y<29;y++)
 	 WriteByte(0x00); 

		 WriteCommand(0xb2); 
	 }


}
//********************************************************************************
//           整屏清屏
//--------------------------------------------------------------------------------
void  ClrScr()
{
    uchar i=0,j=0;
    WriteByte(0);   //写入地址高位
    WriteByte(0);    //写入地址低位
    WriteCommand(0x24);          //地址设置
            
    for(j=0;j<128;j++)          
        for(i=0;i<30;i++)
    	{   
            WriteCommand(0xb0);         //自动读数据 
            WriteByte(0x00);            //清零
            WriteCommand(0xb2);          //自动复位
         }
}
