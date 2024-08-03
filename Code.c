#include <stdio.h>
#include <LPC17xx.h>
#include <math.h>
#include<string.h>
#define PRESCALE 29999999
#define RS_CTRL 0x08000000 // P0.27 To inform whether it is command or data
#define EN_CTRL 0x10000000 // P0.28 Enable Pin first goes high then low
#define DATAL 0x07800000 // P0.23 to P0.26 data lines
#define TRIGPIN (1 << 15) // P0.15
#define ECHOPIN (1 << 16) // P0.16
char ans[50] = "";
int temp, temp1, temp2 = 0;
int flag = 0, fl;
int i, j, k, l, r, time = 5000;
float d = 0;
void clear_ports(void);
void lcdwrite(void);
void portwrite(void);
void lcd_display(unsigned char *buf1);
void delay(unsigned int r1);
void clearDisplay(void);
void startTimer0(void);
float stopTimer0();
void initTimer0(void);
void delaym(int microseconds) {
 
 LPC_TIM0->MR0 = microseconds;
 LPC_TIM0->MCR |= (1 << 0) | (1 << 1);
7
 LPC_TIM0->TCR = 1;
 while (!(LPC_TIM0->IR & (1 << 0)));
 LPC_TIM0->IR = 1;
 LPC_TIM0->TCR = 0;
}
void initTimer0(void)
{
 // Timer for distance
 LPC_TIM0->CTCR = 0x0;
 LPC_TIM0->PR = 11999999;
 LPC_TIM0->TCR = 0x02; // Reset Timer
}
void startTimer0()
{
 LPC_TIM0->TCR = 0x02; // Reset Timer
 LPC_TIM0->TCR = 0x01; // Enable timer
}
float stopTimer0()
{
 LPC_TIM0->TCR = 0x0;
 return LPC_TIM0->TC;
}
void clear_ports(void)
{
 /* Clearing the lines at power on */
 LPC_GPIO0->FIOCLR = DATAL; // Clearing data lines
 LPC_GPIO0->FIOCLR = RS_CTRL; // Clearing RS line
 LPC_GPIO0->FIOCLR = EN_CTRL; // Clearing Enable line
 delaym(3200);
 return;
}
8
void portwrite() {
 int j;
 LPC_GPIO0
->FIOPIN = temp2 << 23;
 if (flag1 == 0)
 
{
 LPC_GPIO0
->FIOCLR = 1 << 27;
 
}
 else
 
{
 LPC_GPIO0
->FIOSET = 1 << 27;
 
}
 LPC_GPIO0
->FIOSET = 1 << 28;
 for (j = 0; j < 90000; j++)
 
;
 LPC_GPIO0
->FIOCLR = 1 << 28;
 for (j = 0; j < 100000; j++)
 
;
}
void lcdwrite() {
 temp2 = (temp1 >> 4) & 0xF;
 portwrite();
 temp2 = temp1 & 0xF;
 portwrite(); }
void blowbuzzer(){ if(d < 6){
 
 LPC_GPIO0
->FIOSET=1<<17;
 
}
 else
9
 {
 
 LPC_GPIO0->FIOCLR=1<<17;
 }
}
void displaydist(){
while (ans[i] != '\0')
 {
 temp1 = ans[i];
 lcdwrite();
 for(j=0;j<30000;j++);
 i++;
 }
}
int measuredist(){
LPC_GPIO0->FIOSET = 0x00000800;
 // Output 10us HIGH on TRIG pin
 LPC_GPIO0->FIOMASK = 0xFFFF7FFF;
 LPC_GPIO0->FIOPIN |= TRIGPIN;
 delaym(10);
 LPC_GPIO0->FIOCLR |= TRIGPIN;
 LPC_GPIO0->FIOMASK = 0x0;
 while (!(LPC_GPIO0->FIOPIN & ECHOPIN)){ // Wait for a HIGH on ECHO pin
 } 
 startTimer0();
 //LPC_GPIO0->FIOSET = LED_Pinsel << 4;
 //echoTime--;
 while (LPC_GPIO0->FIOPIN & ECHOPIN); // Wait for a LOW on ECHO pin
 time = stopTimer0(); // Stop Counting
return time;
10
}
int main()
{
 
 int command[] = {3, 3, 3, 2, 2, 0x01, 0x06, 0x0C, 0x80};
 
 SystemInit();
 SystemCoreClockUpdate();
 initTimer0();
 
 LPC_PINCON->PINSEL0 &= 0x3fffffff; // Interface TRIG P0.15
 LPC_PINCON->PINSEL1 &= 0xfffffff0; // Interface ECHO P0.16
 LPC_GPIO0->FIODIR |= TRIGPIN | 1<<17; // Direction for TRIGGER pin
 LPC_GPIO1->FIODIR |= 0 << 16; // Direction for ECHO PIN
 
 LPC_PINCON->PINSEL1 |= 0;
 LPC_GPIO0->FIODIR |= 0XF << 23 | 1 << 27 | 1 << 28;
 fl = 0;
 for (i = 0; i < 9; i++)
 {
 temp1 = command[i];
 lcdwrite();
 for(j=0;j<30000;j++);
 }
 fl = 1;
 i = 0;
 flag = 1;
 LPC_GPIO0->FIOCLR |= TRIGPIN;
 while (1)
 {
 time=measuredist();
 d = (0.0343 * time)*2*2 / 400;
11
 sprintf(ans, " Distance:%.2f ", d);
 flag1 = 1;
 i = 0;
 flag1 = 0;
 temp1 = 0x01;
 lcdwrite();
 flag1 = 1;
 displaydist();
blowbuzzer();
 
 delaym(88000);
 }
}
