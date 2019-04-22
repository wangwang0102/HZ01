#include <AT89X52.h>
#include <intrins.h>
//12864�������Ŷ���
sbit DI = P2 ^ 2; //����\ָ��ѡ������
sbit RW = P2 ^ 1; //��\дѡ������
sbit E= P2 ^ 0;   //��\дʹ������
sbit CS1 = P2 ^ 4;    //Ƭѡ1����
sbit CS2 = P2 ^ 3;    //Ƭѡ2����
sbit BUSY= P1 ^ 7;    //æ��־λ
//�������ƶ���
sbit Y1 = P3 ^ 0;
sbit Y2 = P3 ^ 1;
sbit X1 = P3 ^ 3;
sbit X2 = P3 ^ 7;
//ADC0832��������
sbit START=P3^4;
sbit OE=P3^6;
sbit EOC=P3^5;

unsigned int ADdata;    //AD�ɼ�ֵ
unsigned int Ldata;
unsigned char ye,lei,shu;
unsigned char ADViewdata[91]; //AD��ʾ���ݴ洢��

char code FrameData[]={	             //��ʾ�ַ��洢��																	 
0x00,0x00,0x3F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x01,0x00,
0x01,0x00,0x11,0x10,0x11,0x08,0x21,0x04,0x41,0x02,0x81,0x02,0x05,0x00,0x02,0x00,    //ʾ
0x00,0x20,0x20,0x20,0x10,0x20,0x13,0xFE,0x82,0x22,0x42,0x24,0x4A,0x20,0x0B,0xFC,
0x12,0x84,0x12,0x88,0xE2,0x48,0x22,0x50,0x22,0x20,0x24,0x50,0x24,0x88,0x09,0x06,		//��		
0x00,0x00,0x3E,0x7C,0x22,0x44,0x22,0x44,0x3E,0x7C,0x01,0x20,0x01,0x10,0xFF,0xFE,
0x02,0x80,0x0C,0x60,0x30,0x18,0xC0,0x06,0x3E,0x7C,0x22,0x44,0x22,0x44,0x3E,0x7C,    //��
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,    //" "
};
//ADת�����
void ADCChage()
{
  START=1;
  START=0;
  while(EOC==0)  //�ȴ�ת�����
 {
   OE=1;
 }
  ADdata = P0;   //��ȡAD����
	OE=0;
}
//���12864Һ��״̬             		  
void CheckState()
{
  DI=0;
	RW=1;
	do
	{
	  E=1; 
	  E=0;
   //������7λΪ0ʱ�ſɲ���(�б�busy�ź�)
	}while(BUSY==1);
}
//��12864д��һ���ֽڵ�����
void WriteCommand(unsigned char cmd) 	
{
	CheckState();         //��鵱ǰ��12864״̬
	DI = 0;
	RW = 0;
	P1 = cmd;             //�ͳ���Ӧ������
	E = 1;
	E = 0;
}  
//��12864д��һ���ֽڵ�����
 void WriteData(unsigned char dat)		
{
   CheckState();   //��鵱ǰ��12864״̬
   DI = 1; 
   RW = 0;
   P1 = dat;       //�ͳ���Ӧ������
   E = 1; 
   E = 0; 
}
//12864Һ����ѡ���������
void LCMCSControl(unsigned int csl)
{
  if(csl==1)    //���ݲ�����ͬ�жϵ�ǰ��12864��������״̬
  { 
    CS1=0,
    CS2=1;
  }
  if(csl==2)
  {
    CS1=1,
    CS2=0;
  }
  if(csl==3)
  {
    CS1=0,
    CS2=0;
  }
}
//12864��ʾ����
void LCMView()
{
  LCMCSControl(Ldata);     //�ȷ��Ϳ�������
  WriteCommand(ye);
  WriteCommand(lei);
  WriteData(shu);         //Ȼ��������
}
//12864����������
void CleanScreen()						
{
	unsigned char page,i;
	LCMCSControl(3);
	for(page=0xb8;page<=0xbf;page++)
	{   
		WriteCommand(page);
		WriteCommand(0x40);
		for(i=0;i<64;i++)
    {
		  WriteData(0x00);
    }
	}
	LCMCSControl(1);
  lei=0x40;
	for(ye=0xb8;ye<0xbf;ye++)
	{
	 shu=0xff;
	 LCMView();
	}
	ye=0xb8;
	for(lei=0x40;lei<=0x7f;lei++)
	{
	  shu=0x80;
	  LCMView();
	}
	ye=0xbf;
	for(lei=0x40;lei<=0x7f;lei++)
	{
	  shu=0x01;
	  LCMView();
	}
  LCMCSControl(2);
  ye=0xb8;
	for(lei=0x40;lei<=0x5b;lei++)
  {
	  shu=0x80;
	  LCMView();
	}
	ye=0xbf;
	for(lei=0x40;lei<=0x5b;lei++)
	{
	  shu=0x01;
	  LCMView();
	}
	lei=0x5b;
	for(ye=0xb9;ye<=0xbe;ye++)
	{
	 shu=0xff;
	 LCMView();
	} 
}
//12864�ĳ�ʼ������
void InitLCM(void)   
{	
  WriteCommand(0xc0);
	WriteCommand(0x3f);
}
//50us����ʱ����
void Delay50us(unsigned int t)
{
 unsigned char j;  
 for(;t>0;t--)   
  for(j=19;j>0;j--);
}
//ˢ��12864Һ��
void RefreshLCM()
{
  unsigned char i;
  for(i=0xb9;i<=0xbe;i++)
  {
    ye=i;
	  shu=0x00;
	  LCMView();
	}
}
//������
void main()
{
    unsigned int r,j,q,k;
    unsigned int Xaxis =0;
    unsigned int Yaxis = 1;
    unsigned char l;
    unsigned char d1,d2,d3,d4,d5;
    CleanScreen();
    InitLCM();
	  LCMCSControl(2);
	  l=0xb8;
	  for(k=0;k<4;k++,l=l+0x02)         //������ʾ�Ҳ����ʾ
	  {
	    ye=l;
	    lei=0x70;
	    for(r=0;r<16;r++)
      {
        shu=FrameData[2*r+1+32*k];
	      LCMView();
	      lei++;
	    }
	    ye=l+0x01;
	    lei=0x70;
	    for(r=0;r<16;r++)
      { 
        shu=FrameData[2*r+32*k];
	      LCMView();
	      lei++;
	    }
	}
  while(1)
  {
    while(X2==0)      //����X��
    {
      while(X2==0);
      Xaxis = Xaxis + 1;
    }
    while(X1==0)
    {
      while(X1==0);
      if(Xaxis!=0) 
      {
        Xaxis = Xaxis - 1;
      }
    }
    while(Y1==0)     //����Y��
    {
      while(Y1==0);
      Yaxis = Yaxis + 1;
    }
    while(Y2==0)
    {
      while(Y2==0);
      if(Yaxis!=1) 
      {
        Yaxis=Yaxis-1;
      }
    }
    for(j=0;j<90;j++) //AD�������ֵ
    {
      ADCChage();
      ADViewdata[j]=ADdata;
      if(ADViewdata[j]>ADViewdata[91])
      {
        ADViewdata[91]=ADViewdata[j];
      }
      Delay50us(Xaxis);
    }
    while(ADdata!=ADViewdata[91])    //����ɼ�ֵ����ȣ������
    {
      ADCChage();
    }
    for(j=0;j<90;j++)      //��������90��
    {
       ADCChage();
       ADViewdata[j]=ADdata; 
       Delay50us(Xaxis);
    }
    lei=0x41;
    for(r=0,j=0;r<90;r++,j++)
	  {
   	  if(j<63)
      { 
        Ldata=1;
      }
	    if(j==63)
      { 
        lei=0x40;
      }
	    if(j>=63)
      {
        Ldata=2;
      }
	    RefreshLCM();     //ˢ�µ�ǰ��ʾ
	    if(ADViewdata[j>=127])//����ѹ
	    {                             
        ADdata=(ADViewdata[j]-127)*0.196/Yaxis;  //�����ѹֵ
        if(ADdata<=7)       
        {
          ye=0xbb;
          shu=(0x80>>ADdata);
        }
	      else if(ADdata<=15) 
        {
          ye=0xba;
          shu=(0x80>>(ADdata-8));
        }
        else if(ADdata<=23)
        {
          ye=0xb9;
          shu=(0x80>>(ADdata-16));
         }
        else if(ADdata<=31) 
        {
          ye=0xb9;
          shu=(0x80>>(ADdata-24));
        }
  	  }
	    if(ADViewdata[j]<127)     //����ѹ
	    {
	        ADdata=(127-ADViewdata[j])*0.196/Yaxis;    //�����ѹֵ
          if(ADdata<=7)
          {
              ye=0xbc;
              shu=(0x01<<(ADdata));
          }
          else if(ADdata<=15)
          {
              ye=0xbd;
              shu=(0x01<<(ADdata-8));
          }
          else if(ADdata<=23) 
          {
              ye=0xbe;
              shu=(0x01<<(ADdata-16));
          }
	        else if(ADdata<=31)
          {
              ye=0xbe;
              shu=(0x01<<(ADdata-24));
          }
	    }
      if(r==0)//�ж�����
	    {
	      d1=shu;
	      d2=ye;
	     }
	     if(r!=0)
	    {
	      d3=shu;
	      d4=ye;
	      if(ye==d2)  //�����ȣ����ж��Ƿ���ʾ���
	      {
	        if(shu>d1)
	        {
	          d5=shu;
	          d5=d5>>1;
	          while(d5!=d1)
	          {
              d5=d5>>1;
	            shu=shu|(shu>>1);
            }
	        }
	        if(shu<d1)
	        {
	          d5=shu;
	          d5=d5<<1;
	          while(d5!=d1)
	         {
              d5=d5<<1;
              shu=shu|(shu<<1);
           } 
	        }
	      }
        if(ye<d2)
	      {  
	        for(q=0;q<7;q++) 
	        {
		        shu=shu|(shu<<1);
		      } 
		    LCMView();
	 	    ye++;
		    while(ye<d2) 
        {
          shu=0xff;
          LCMView();
          ye++;
        }
		    if(ye==d2)
        {
		        shu=0x01;
					  if(shu<d1)
	          {
	            d5=shu;
	            d5=d5<<1;
	            while(d5!=d1)
	            {
                d5=d5<<1;
                shu=shu|(shu<<1);
               } 
	           }
					  }
        }
	      if(ye>d2)
	      {
	        for(q=0;q<7;q++) 
	        {
		        shu=shu|(shu>>1);
		      }
		    LCMView();
	 	    ye--;
		    while(ye>d2) {shu=0xff,LCMView(),ye--;}
		    if(ye==d2)
        {
		        shu=0x80;
					  if(shu>d1)
	          {
	            d5=shu;
	            d5=d5>>1;
	            while(d5!=d1)
	            {
                d5=d5>>1;
                shu=shu|(shu>>1);
               } 
	           }
				} 
      }
	  }
	  if(r!=0)
	  {
	    d1=d3;
	    d2=d4;
	  }
    LCMView();
    if(lei!=0x7f)
    {
      lei++;
    }
   }
	}
}
	
	  
   
	