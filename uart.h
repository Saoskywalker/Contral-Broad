 void CPU_Init()
 { 
  SCON=0x50;
 // T2CON=0x34;
 // RCAP2H=(65536-13)/256;
 // RCAP2L=(65536-13)%256;
  TMOD=0x21;
  TH1=0xff;
  TL1=0xff;
  PCON|=0x80;
  TR1 =1;
  
  //PS=1;
  ES=1;   
	ET1 = 1;  //÷–∂œ‘ –Ì 

  	TH0 =(65536-500)/256;
	TL0 =(65536-500)%256;
	ET0=1;

	S2CON=0xda;
	BRT=-(22118400/12/32/2400);
	AUXR=0x10;
	IE2=0x01;
  	TR0 =1;
//	PT0=1;
	EA  = 1;  
 }