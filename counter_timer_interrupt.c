/*
 * counter_timer_interrupt.c
 *
 * Created: 4/8/2019 3:17:32 PM
 * Author : Sahil S. Mahajan
 * Description: Interrupt based counter using one timer for the display and another timer for increments. 
 *                  Increments once per second.
 *
 */

#include <REG51F.H>

void init(void);
void configureTimer0(void);
void configureTimer1(void);
void enableInterrupt(void);
void resetTimer0(void);
void resetTimer1(void);
void display(void);
void incrementOnes(void);
void incrementTens(void);
void incrementHundreds(void);
void incrementThousands(void);
void resetToZero(void);

#define TRIGGER 20          //the number of timer overflow iterations before incrementing count

sbit sl1 = P2^3;
sbit sl2 = P2^2;
sbit sl3 = P2^1;
sbit sl4 = P2^0;

unsigned int numberList[10] = {
                                  0xFC,   //0         
                                  0x60,   //1
                                  0xDA,   //2 
                                  0xF2,   //3
                                  0x66,   //4
                                  0xB6,   //5
                                  0xBE,   //6
                                  0xE0,   //7
                                  0xFE,   //8
                                  0xF6    //9
                              };

unsigned int numbersToDisplay[4] = {'0','0','0','0'};   //the thousands, hundreds, tens, and ones place of the count number

                            
unsigned int digitSelector[4][4] = {
                                      0,1,1,1,   //select 1st digit on 7seg display
                                      1,0,1,1,   //select 2nd digit on 7seg display
                                      1,1,0,1,   //select 3rd digit on 7seg display
                                      1,1,1,0    //select 4th digit on 7seg display
                                   };                 

unsigned int x;         //used with digitSelctor variable in display function to select digit
unsigned int y;         //used in timer 1 interrupt to count iterations of timer overflow before incrementing count




void main()
{
    init();
    configureTimer0();
    configureTimer1();
    enableInterrupt();
    while(1);
}


/* -----------------
 * Function: init
 * -----------------
 *
 * clears display, initializes variables, and configures timer mode
 *
 */

void init(void)
{
    P0 = 0x00;      //clear display
    x = 0;
    TMOD = 0x11;    //use Timer 0 in 16-bit Timer operating mode and Timer 1 in 16-bit Timer operating mode  
}


/* -----------------
 * Function: configureTimer0
 * -----------------
 *
 * sets up Timer 0 and enables timer 0 interrupt
 *
 */

void configureTimer0(void)
{
    resetTimer0();          //load timer0 start value
    TR0 = 1;                //start Timer 0
    ET0 = 1;                //enable Timer 0 overflow interrupt
}


/* -----------------
 * Function: configureTimer1
 * -----------------
 *
 * sets up Timer 1 and enables timer 1 interrupt
 *
 */

void configureTimer1(void)
{
    resetTimer1();          //load timer1 start value
    TR1 = 1;                //start Timer 1
    ET1 = 1;                //enable Timer 1 overflow interrupt
}



/* -----------------
 * Function: enableInterrupt
 * -----------------
 *
 * enables all interrupts that has their individual interrupt bit enabled
 *
 */

void enableInterrupt(void)
{
    EA = 1;
}


/* -----------------
 * Function: resetTimer0
 * -----------------
 *
 * loads Timer 0's 16 bit count register with start value
 *
 */

void resetTimer0(void)
{
    TH0 = 0xEF;
    TL0 = 0xCD;         //set to 4.5 milliseconds  
}


/* -----------------
 * Function: resetTimer1
 * -----------------
 *
 * loads Timer 1's 16 bit count register with start value
 *
 */

void resetTimer1(void)
{
    TH1 = 0x4C;
    TL1 = 0x00;         //set to 50 milliseconds  
}


/* -----------------
 * Function: display
 * -----------------
 *
 * selects one digit on the 7-segment display at a time and displays a number on the selected digit
 *
 */

void display(void)
{

    sl1 = digitSelector[x][0];
    sl2 = digitSelector[x][1];
    sl3 = digitSelector[x][2];
    sl4 = digitSelector[x][3];   /* Digit is selected when its pin is reset.        */ 
                                 /* Only one of these is selected in each iteration */

    P0 = numberList[numbersToDisplay[x]-'0'];

    x++;

    if(x == 4)
    {
        x = 0;
    }

}


/* -----------------
 * Function: incrementOnes
 * -----------------
 *
 * 1. Increments ones place of the count number
 * 2. Calls incrementTens function if ones place increments past 9
 *
 */

void incrementOnes(void)
{
    numbersToDisplay[3]++;   
    if(numbersToDisplay[3] == '9'+1)
    {
        incrementTens();
    }   
}


/* -----------------
 * Function: incrementTens
 * -----------------
 *
 * 1. Resets ones place to zero
 * 2. Increments tens place of the count number
 * 3. Calls incrementHundreds function if tens place increments past 9   
 * 
 */

void incrementTens(void)
{
    numbersToDisplay[3] = '0';
    numbersToDisplay[2]++;
    if(numbersToDisplay[2] == '9'+1)
    {
        incrementHundreds();
    }
}


/* -----------------
 * Function: incrementHundreds
 * -----------------
 *
 * 1. Resets tens place to zero
 * 2. Increments hundreds place of the count number
 * 3. Calls incrementThousands function if hundreds place increments past 9   
 * 
 */

void incrementHundreds(void)
{
    numbersToDisplay[2] = '0';
    numbersToDisplay[1]++;
    if(numbersToDisplay[1] == '9'+1)
    {
        incrementThousands();
    }
}


/* -----------------
 * Function: incrementThousands
 * -----------------
 *
 * 1. Resets hundreds place to zero
 * 2. Increments thousands place of the count number
 * 3. Calls resetToZero function if thousands place increments past 9   
 * 
 */

void incrementThousands(void)
{
    numbersToDisplay[1] = '0';
    numbersToDisplay[0]++;
    if(numbersToDisplay[0] == '9'+1)
    {
        resetToZero();
    }
}


/* -----------------
 * Function: resetToZero
 * -----------------
 *
 * resets count to 0000   
 * 
 */

void resetToZero(void)
{
    numbersToDisplay[0] = '0';
    numbersToDisplay[1] = '0';
    numbersToDisplay[2] = '0';
    numbersToDisplay[3] = '0';
}



/* --------------------------------------
 * Interrupt Service Routine: Timer 0 Interrupt
 * --------------------------------------
 *
 * Runs each time timer 0 interrupt is generated 
 *
 * resets Timer 0's count register to start value, and calls display function
 *
 */

void isr_t0() interrupt 1
{
    resetTimer0();
    display();
}


/* --------------------------------------
 * Interrupt Service Routine: Timer 1 Interrupt
 * --------------------------------------
 *
 * Runs each time timer 1 interrupt is generated 
 *
 * resets Timer 1's count register to start value, and increments count number when certain number of iterations has been reached
 *
 */

void isr_t1() interrupt 3
{
    resetTimer1();
    y++;
    if(y==TRIGGER)
    {
        incrementOnes();
        y = 0;
    }
}