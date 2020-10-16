
#include "msp.h"

#include <stdio.h>

#include <stdlib.h>





void setupKeypad(void){

    //columns setup
    P3-> SEL0 &= ~(BIT5|BIT6|BIT7);
    P3-> SEL1 &= ~(BIT5|BIT6|BIT7);
    P3-> DIR &= ~(BIT5|BIT6|BIT7);
    P3-> REN |= (BIT5|BIT6|BIT7);
    P3-> OUT |= (BIT5|BIT6|BIT7);

    //Rows setup
    P5-> SEL0 &= ~(BIT0|BIT2);
    P5-> SEL1 &= ~(BIT0|BIT2);
    P5-> DIR &= ~(BIT0|BIT2);
    P5-> REN |= (BIT0|BIT2);
    P5-> OUT |= (BIT0|BIT2);

    P1-> SEL0 &= ~(BIT6|BIT7);
    P1-> SEL1 &= ~(BIT6|BIT7);
    P1-> DIR &= ~(BIT6|BIT7);
    P1-> REN |= (BIT6|BIT7);
    P1-> OUT |= (BIT6|BIT7);

}




int getKeypress(void) {

    setupKeypad();  //sets everything back to pull up resistor every time readKeypad is called



    //set P4.0 to output = 0

    P3 ->DIR |= BIT5;

    P3 -> OUT &= ~BIT5;



//check if a button in that row is pressed

    if(!(P5->IN &BIT2))
    {
        while(!(P5->IN &BIT2)); //sits at this line while the button is pressed - allows for a single button press
        return 0;   //choose 91 because not acceptable and then can change after determining what actually is
    }



    if(!(P5->IN &BIT0))
    {
        while(!(P5->IN &BIT0));
        return 8;
    }



    if(!(P1->IN &BIT7))
    {
        while(!(P1->IN &BIT7));
        return 5;
    }



    if(!(P1->IN &BIT6))
     {
         while(!(P1->IN &BIT6));
         return 2;
     }



    //set P4.0 back to pull-up resistor

    P3 ->DIR &= ~BIT5;

    P3 -> OUT |= BIT5;

    __delay_cycles(20);



    //set P4.2 to an output = 0

    P3 ->DIR |= BIT6;

    P3 -> OUT &= ~BIT6;



    //read each row

    if(!(P5->IN &BIT2))

    {

        while(!(P5->IN &BIT2)); //sits at this line while the button is pressed - allows for a single button press

        return 20;   //choose 91 because not acceptable and then can change after determining what actually is

    }



    if(!(P5->IN &BIT0))

    {

        while(!(P5->IN &BIT0));

        return 7;

    }



    if(!(P1->IN &BIT7))

    {

        while(!(P1->IN &BIT7));

        return 4;

    }



    if(!(P1->IN &BIT6))

     {

         while(!(P1->IN &BIT6));

         return 1;

     }



    P3-> DIR &=~ BIT6;

    P3-> OUT |=  BIT6;

    __delay_cycles(20);



    //set P6.1 to an output = 0

     P3 ->DIR |= BIT7;

     P3 -> OUT &= ~BIT7;



     //read each row

     if(!(P5->IN &BIT2))

     {

         while(!(P5->IN &BIT2)); //sits at this line while the button is pressed - allows for a single button press

         return 30;   //choose 91 because not acceptable and then can change after determining what actually is

     }



     if(!(P5->IN &BIT0))

     {

         while(!(P5->IN &BIT0));

         return 9;

     }



     if(!(P1->IN &BIT7))

     {

         while(!(P1->IN &BIT7));

         return 6;

     }



     if(!(P1->IN &BIT6))

      {

          while(!(P1->IN &BIT6));

          return 3;

      }





     //set P6.1 Back to High

      P3 ->DIR &= ~BIT7;

      P3 -> OUT |= BIT7;

      __delay_cycles(20);



      //if no button is pressed return -1

      if((P5->IN &BIT2)|(P5->IN &BIT0)|(P1->IN &BIT7)|(P1->IN &BIT6))

      {

          return -1;

      }

}
