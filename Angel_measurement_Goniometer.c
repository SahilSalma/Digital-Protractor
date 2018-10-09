    // code         901  bytes 
    // data          48  bytes  
    // cheksum        E2AA/0x0013 after code and eeprom protected
    // confing        0x3F94 before code lock
    //  do              0x3E14 after code protect
    //                PIC16F676

 

#include<pic.h>


unsigned char display[17]   = "Angle =     dgr ";
const unsigned char title[17]="DigiPro by Sahil";
#define en  RA1                       
#define rs  RA0 

void MSDelay(unsigned int);
void delay();
void small_delay();

void lcdcmd_4bit  (unsigned char value);
void lcddata_4bit (unsigned char value);
 

void small_delay()
{unsigned char i;
 for(i=0; i<10; i++);
}


void main()
{unsigned int temp;  
 unsigned char i;
 unsigned char thou,hund,tens,unit,correction,offset;
 unsigned int angle_magnitude,degree;

 

//1  VDD                             +5v
//2  RA5/T1CKI/OSC1/CLKIN           spare
//3  RA4/T1G/OSC2/AN3/CLKOUT        spare
//4  RA3/MCLR/VPP                  ip button(only input pin) 
//5  RC5                            spare (do not use for any purpose in this application) 
//6  RC4                            spare ------- do -----------
//7  RC3/AN7                       op LCD data ms-bit b3 
//14 VSS                             grd
//13 RA0/AN0/CIN+/ICSPDAT          op LCD cmd rs  
//12 RA1/AN1/CIN-/VREF/ICSPCLK     op LCD cmd en 
//11 RA2/AN2/COUT/T0CKI/INT        ip AN2 
//10 RC0/AN4                       op LCD data ls-bit b0 
//9  RC1/AN5                       op LCD data        b1 
//8  RC2/AN6                       op LCD data        b2 

    
   ANSEL = 0x80; // 1000 0000 analog selection on AN7 RC3 pin7  remaining pins  digital io
  
   TRISC0 = 0; // op RC0 pin10 LCD data bit0 
   TRISC1 = 0; // op RC1 pin9  LCD data bit1
   TRISC2 = 0; // op RC2 pin8  LCD data bit3 
   TRISC3 = 0; // op RC3 pin7  LCD data bit4 
   TRISC4 = 1; // do not use this pin for any purpose  
   TRISC5 = 1; //        do  
   
  
   TRISA0 = 0; // op RA0 pin13  rs 
   TRISA1 = 0; // op RA1 pin12  en 
   TRISA2 = 1; // ip RA2 pin11  AN2
   TRISA3 = 1; // ip button pin4  
   TRISA4 = 0; // op spare  pin3
   TRISA5 = 0; // op spare  pin2  
  
   
   CM0 = 1;  // comparator off RA0 RA1 RA2 as io
   CM1 = 1;  //    do
   CM2 = 1;  //    do
   T0CS = 0; // RA2 as ip LCD data bit2
   TMR1CS = 0;  // RA5 as digital io 
      GIE = 0; // global interrupt disabled
      
      
     ADFM = 0;// left  justified
     VCFG = 0;// ref = vdd = 5v
             
     ADCS2 = 0; // ADC clock = fosc/32 
     ADCS1 = 1; //  do 
     ADCS0 = 0; //  do
     
     ////  start of   initialising for 4 bit programming 
	

	MSDelay(20);	
    rs = 0;
    MSDelay(20);
	en = 1;
	MSDelay(1);
    //P2 = 0x30;	// 0011 0000	
    RC0 = 1;
    RA1 = 1;
    RA2 = 0;
    RA3 = 0;
    MSDelay(15);
   	en = 0;

	 
	 MSDelay(1);
	 en = 1;
	 MSDelay(1);
	// P2 = 0x30;	
	RC0 = 1;
    RC1 = 1;
    RC2 = 0;
    RC3 = 0;	
     MSDelay(2);
     en = 0;

	 
     MSDelay(1);
	 en = 1;
	 MSDelay(1);
	 //P2 = 0x30;	
	RC0 = 1;
    RC1 = 1;
    RC2 = 0;
    RC3 = 0;	
     MSDelay(2);
     en = 0;

	 
	 MSDelay(1);
	 en = 1;
	 MSDelay(1);
	// P2 = 0x20;	// 0010 0000	
	RC0 = 0;
    RC1 = 1;
    RC2 = 0;
    RC3 = 0;
	 MSDelay(2);
     en = 0;
//  end  of   initialising for 4 bit programming 
     
 	 lcdcmd_4bit(0x28);	// 5x7 2x16
	 lcdcmd_4bit(0x0C);	// 0x0C dis on cursor blinking
	 lcdcmd_4bit(0x01);// 0x01 clrscr
	 lcdcmd_4bit(0x80); // cursor at 0 0 
   for(i=0;i<16;i++)
   {
   lcddata_4bit(title[i]);
   
   }

      
      MSDelay(2000); 
 

 while(1) // super while
  { 
      //codes for ADC
      

    ADON = 1;
    small_delay();


     CHS2 = 0; // to select ch2 AN2 for angle potentio meter
     CHS1 = 1; //   do
     CHS0 = 0; //   do
     
    small_delay();
 // for(loopcounter=0; loopcounter<10; loopcounter++);
     
    
    ADCON0 = 0b00001011;  // start of conversion of ref 
    while(ADCON0 == 0b00001011);// wait till end of conversion of ref
    ADIF = 0;

    small_delay();
  
            
    angle_magnitude = ADRESH*4 + ADRESL/64; // left justify 10 bits 
    // now we get angle_magnitude = 690  at 180 degree of pot rotation
    // 690/180 = 3.83
    // to obtain degree we require degree = angle_magniyude/3.83;
    // compiler does not support for above subroutine as it demands more memory
    // following codes are wayout 
    // if we raise 690 to 720 we can use "degree = (angle_magnitude + offset)/4"
    // difference = 720-690 = 30
    // 23 = 690/30 
    // 23 steps needs one step to be added 
    // following codes do that 
    // after doing that " degree = (angle_magnitude + offset)/4" is used  
    
    offset = 0;
    for(temp = angle_magnitude,correction = 0; temp>0;temp--,correction++)
    {
      if(correction == 23)
      {
        offset++;
        correction = 0;
      }
    
    }
    
        
    degree = (angle_magnitude + offset)/4; 
   // thou =  degree/1000;
   // temp =  degree%1000;
    hund =  degree/100;
    temp =  degree%100;
    tens =  temp/10;
    unit =  temp%10;
    
    
   // lcddata_4bit(thou+'0');
   display[8] = hund+'0';
   display[9] = tens+'0';
   display[10]= unit+'0';
    
    lcdcmd_4bit(0x01);
    lcdcmd_4bit(0x80);
    
    for(i=0;i<16;i++)
    {
      lcddata_4bit(display[i]);
    }


    MSDelay(1000);
      
   

  } // end of super while 


}

void MSDelay(unsigned int ms)
{
  unsigned int i,j;
  for(i=0; i<ms; i++)
  {
    for(j=0; j<100;j++);
  
  }

}

void lcdcmd_4bit (unsigned char value)
{
	unsigned char highnibble,lownibble;	 
	 highnibble = (value/16);
	 lownibble =  value % 16;

//	lownibbleonport = lownibble * 16;
 //   highnibbleonport = highnibble * 16;
	
	 
	 MSDelay(10);
	 rs = 0;
	 en = 1;	
     MSDelay(10);
//	 P2 = highnibbleonport;
     PORTC = highnibble;
	 MSDelay(10);
     en = 0;
	 MSDelay(10);
     MSDelay(10);
	 rs = 0;
	 en = 1;
     MSDelay(10);
//	 P2 = lownibbleonport;
     PORTC = lownibble;
	 MSDelay(10);
     en = 0;
	 

	
}

void lcddata_4bit (unsigned char value)
{
   unsigned char highnibble,lownibble;
  highnibble = value/16;
	lownibble =  value % 16;

//	  lownibbleonport  =  lownibble * 16;
 //   highnibbleonport =  highnibble * 16;
	
	 
	 
		MSDelay(10);
	
	 rs = 1;
	 en = 1;	
     MSDelay(10);
//	 P2 = highnibbleonport;
     PORTC = highnibble;
	 MSDelay(10);
     en = 0;
	 MSDelay(10);
	 rs = 1;
	 en = 1;
     MSDelay(10);
//	 P2 = lownibbleonport;
     PORTC = lownibble;
	 MSDelay(10);
     en = 0;

	
}







	/*	LCD Display for 4 bits 	*/

//#include <REGX51.H>
//
//sbit rs = P0^0;
//sbit rw = P0^1; //permenant grd
//sbit en = P0^2;
//
//
//void lcdcmd_4bit (unsigned char value);
//void lcddata_4bit (unsigned char value);
//void MSDelay (unsigned int itime);
//
//unsigned char alpha,highnibble,lownibble,lownibbleonport,highnibbleonport;
//
//unsigned char i;
//
//unsigned char test1[16] = "ABCDEFGHIJKLMNOP";
//unsigned char test2[16] = "QRSTUVWXYZabcdef";
//unsigned char test3[16] = "ghijklmnopqrstuv";
//unsigned char test4[16] = "wxyz0123456789@*";
//
//void main()
//{
//    
//
///*	"	Wait for abour 20mS
//"	Send the first init value (0x30)
//"	Wait for about 10mS
//"	Send second init value (0x30)
//"	Wait for about 1mS
//"	Send third init value (0x30)
//"	Wait for 1mS
//"	Select bus width (0x30 - for 8-bit and 0x20 for 4-bit)
//"	Wait for 1mS  */
//
//    
////  start of   initialising for 4 bit programming 
//	rw = 0;
//
//	MSDelay(20);	
//    rs = 0;
//    MSDelay(20);
//	en = 1;
//	MSDelay(1);
//    P2 = 0x30;		
//    MSDelay(15);
//   	en = 0;
//
//	 
//	 MSDelay(1);
//	 en = 1;
//	 MSDelay(1);
//	 P2 = 0x30;		
//     MSDelay(2);
//     en = 0;
//
//	 
//     MSDelay(1);
//	 en = 1;
//	 MSDelay(1);
//	 P2 = 0x30;		
//     MSDelay(2);
//     en = 0;
//
//	 
//	 MSDelay(1);
//	 en = 1;
//	 MSDelay(1);
//	 P2 = 0x20;	// 4 bit	
//	 MSDelay(2);
//     en = 0;
////  end  of   initialising for 4 bit programming 
//
//
//	 lcdcmd_4bit(0x28);	// 5x7 2x16
//	 lcdcmd_4bit(0x0C);	// 0x0C dis on cursor blinking
//	 lcdcmd_4bit(0x01);// 0x01 clrscr
//	 lcdcmd_4bit(0x80); // cursor at 0 0 
//	  
//	
//
//
//	
//
//	
//
// while(1)
// {
//	 lcdcmd_4bit(0x01);// 0x01 clrscr
//	  MSDelay(50);
//	 lcdcmd_4bit(0x80); // cursor at 0 0 
//	    MSDelay(50);
//	 for(i=0;i<16;i++)
//	 {
//   	  lcddata_4bit(test1[i]);
//	  MSDelay(5);
//	 }
//
//	 lcdcmd_4bit(0xC0);   
//	  MSDelay(5);
//	 for(i=0;i<16;i++)
//	 {
//   	  lcddata_4bit(test2[i]);
//	   MSDelay(5);
//	 }
//	  MSDelay(5000);
//
//	  lcdcmd_4bit(0x01);// 0x01 clrscr
//	   MSDelay(5);
//	 lcdcmd_4bit(0x80); // cursor at 0 0 
//	  MSDelay(5);
//	 for(i=0;i<16;i++)
//	 {
//   	  lcddata_4bit(test3[i]);
//	   MSDelay(5);
//	 }
//
//	 lcdcmd_4bit(0xC0); // 
//	  MSDelay(5); 
//	 for(i=0;i<16;i++)
//	 {
//   	  lcddata_4bit(test4[i]);
//	   MSDelay(5);
//	 }
//  	  MSDelay(5000);
//
//
//}
//		
//	
//}
//	
//void lcdcmd_4bit (unsigned char value)
//{
//		 
//	 highnibble = value/16;
//	 lownibble =  value % 16;
//
//	lownibbleonport = lownibble * 16;
//    highnibbleonport = highnibble * 16;
//	
//	 
//	 MSDelay(10);
//	 rs = 0;
//	 en = 1;	
//     MSDelay(10);
//	 P2 = highnibbleonport;
//	 MSDelay(10);
//     en = 0;
//	 MSDelay(10);
//     MSDelay(10);
//	 rs = 0;
//	 en = 1;
//     MSDelay(10);
//	 P2 = lownibbleonport;
//	 MSDelay(10);
//     en = 0;
//	 
//
//	
//}
//
//void lcddata_4bit (unsigned char value)
//{
//   
//  highnibble = value/16;
//	lownibble =  value % 16;
//
//	  lownibbleonport  =  lownibble * 16;
//    highnibbleonport =  highnibble * 16;
//	
//	 
//	 
//		MSDelay(10);
//	
//	 rs = 1;
//	 en = 1;	
//     MSDelay(10);
//	 P2 = highnibbleonport;
//	 MSDelay(10);
//     en = 0;
//	 MSDelay(10);
//	 rs = 1;
//	 en = 1;
//     MSDelay(10);
//	 P2 = lownibbleonport;
//	 MSDelay(10);
//     en = 0;
//
//	
//}	
//
//void MSDelay (unsigned int itime)
//{
//	unsigned int i,j;
//	for(i=0; i<itime; i++)
//	{
//		for(j=0; j<100; j++)
//		{;}
//	}
//}
//

