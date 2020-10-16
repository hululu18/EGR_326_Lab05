/*------------------------------------------------
 *   Author = Dixit Gurung
 *   EGR 326 901
 *   Date = 10/10/2020
 *   Lab_5
 *
 *   Description:Writing and Reading RTC Using I2C
 * Writing sec min hrs day date month year to RTC and reading sec min hrs day date month year and temperature from RTC.
 *---------------------------------------------------*/


/*Pins
 * P6.5 - SCL, P6.4 - SDA
 *
*/


#include <stdio.h>
#include "msp.h"
#include "Keypad.h"

void I2C1_init(void);
int I2C1_burstWrite(int slaveAddr, unsigned char memAddr, int byteCount, unsigned char* data);


void delayMs(int n);
int I2C1_Read(int slaveAddr, unsigned char memAddr, unsigned char* data);
float temp_decimal (unsigned char temp_tens, unsigned char temp_frac);
void print_date_month_year(void);
void print_hours_minutes_seconds(void);
void print_temp(void);

unsigned char sec, min, hrs, day, date, month, year, temp_lsb, temp_msb;

#define SLAVE_ADDR 0x68     // 1101 000.    DS1337

int main(void)
{


    int done = 0;

    /*                                 00    01    02    03    04    05    06 */
    //unsigned char timeDateToSet[15] = {0x55, 0x58, 0x16, 0x05, 0x19, 0x11, 0x15, 0}; // 2015 November 19, Thu, 16:58:55
    unsigned char timeDateToSet[15];
    unsigned char temp_tens,temp_ones;
    int btn1,btn2;

    I2C1_init();


    int keypressed = 0;
    int i = 0, j=0,count=0;
    while (1)
    {
        keypressed = getKeypress();

        if (keypressed != -1)
        {
            if (keypressed == 20)
            {
                i = 0; //reseting the array index after * is pressed

                keypressed = getKeypress();
                while (keypressed == -1)//Wait unltil button is pressed
                {
                    keypressed = getKeypress();
                }
                btn2 = keypressed;
                if (btn2 == 1)
                {
                    print_date_month_year();
                }
                else if (btn2 == 2)
                {
                    print_hours_minutes_seconds();
                }
                else if (btn2 == 3)
                {
                    print_temp();
                }

            }

//For taking input from keypad
            else
            {
                count++;

                if (count == 1)
                {
                    //printf("1 \n");
                    temp_tens = keypressed;
                    temp_tens = temp_tens << 4; //Bit shifting because to store two button presses in single byte

                }
                if (count == 2)
                {
                    count = 0;
                    //printf("2 \n");
                    temp_ones = keypressed;
                    temp_tens = temp_tens | temp_ones;
                    timeDateToSet[i] = (unsigned char) temp_tens;
                    printf(" %02x \n ", timeDateToSet[i]);
                    i++;
                    if (i > 6)//For reading 7 set of inputs(sec min hrs day date month year)
                    {
                        done = 1;//Done is set to 1 after successfully reading all values of (sec min hrs day date month year)

                    }
                }
            }


        }

        //For testing purpose printing the values of stored (sec min hrs day date month year)
        if(done == 1){
            done=0;
            for( j=0;j<7;j++){

                printf("%02x \n",timeDateToSet[j]);
                I2C1_burstWrite(SLAVE_ADDR, 0, 7, timeDateToSet);    /* write the first seven bytes of the registers */

                //read_everything(); //Reads all 7 registers from RTC and prints all terms
            }
        }

    }
}

/* configure UCB1 as I2C */
void I2C1_init(void)
{
    EUSCI_B1->CTLW0 |= 1; /* disable UCB1 during config */
    EUSCI_B1->CTLW0 = 0x0F81; /* 7-bit slave addr, master, I2C, synch mode, use SMCLK */
    EUSCI_B1->BRW = 30; /* set clock prescaler 3MHz / 30 = 100kHz */
    P6->SEL0 |= 0x30; /* P6.5, P6.4 for UCB1 */
    P6->SEL1 &= ~0x30;
    EUSCI_B1->CTLW0 &= ~1; /* enable UCB1 after config */
}

/* Use burst write to write multiple bytes to consecutive locations
 * burst write: S-(slaveAddr+w)-ACK-memAddr-ACK-data-ACK...-data-ACK-P
 */
int I2C1_burstWrite(int slaveAddr, unsigned char memAddr, int byteCount,
                    unsigned char* data)
{
    if (byteCount <= 0)
        return -1; /* no write was performed */

    EUSCI_B1->I2CSA = slaveAddr; /* setup slave address */
    EUSCI_B1->CTLW0 |= 0x0010; /* enable transmitter */
    EUSCI_B1->CTLW0 |= 0x0002; /* generate START and send slave address */
    while ((EUSCI_B1->CTLW0 & 2))
        ; /* wait until slave address is sent */
    EUSCI_B1->TXBUF = memAddr; /* send memory address to slave */

    /* send data one byte at a time */
    do
    {
        while (!(EUSCI_B1->IFG & 2))
            ; /* wait till it's ready to transmit */
        EUSCI_B1->TXBUF = *data++; /* send data to slave */
        byteCount--;
    }
    while (byteCount > 0);

    while (!(EUSCI_B1->IFG & 2))
        ; /* wait till last transmit is done */
    EUSCI_B1->CTLW0 |= 0x0004; /* send STOP */
    while (EUSCI_B1->CTLW0 & 4)
        ; /* wait until STOP is sent */

    return 0; /* no error */
}



/* Read a single byte at memAddr
 * read: S-(slaveAddr+w)-ACK-memAddr-ACK-R-(saddr+r)-ACK-data-NACK-P
 */
int I2C1_Read(int slaveAddr, unsigned char memAddr, unsigned char* data) {
    EUSCI_B1->I2CSA = slaveAddr;    /* setup slave address */
    EUSCI_B1->CTLW0 |= 0x0010;      /* enable transmitter */
    EUSCI_B1->CTLW0 |= 0x0002;      /* generate START and send slave address */
    while((EUSCI_B1->CTLW0 & 2));   /* wait until slave address is sent */
    EUSCI_B1->TXBUF = memAddr;      /* send memory address to slave */
    while(!(EUSCI_B1->IFG & 2));    /* wait till it's ready to transmit */
    EUSCI_B1->CTLW0 &= ~0x0010;     /* enable receiver */
    EUSCI_B1->CTLW0 |= 0x0002;      /* generate RESTART and send slave address */
    while(EUSCI_B1->CTLW0 & 2);     /* wait till restart is finished */
    EUSCI_B1->CTLW0 |= 0x0004;      /* setup to send STOP after the byte is received */
    while(!(EUSCI_B1->IFG & 1));    /* wait till data is received */
    *data = EUSCI_B1->RXBUF;        /* read the received data */
    while(EUSCI_B1->CTLW0 & 4) ;    /* wait until STOP is sent */
    return 0;                       /* no error */
}

/* system clock at 3 MHz */
void delayMs(int n) {
    int i, j;

    for (j = 0; j < n; j++)
        for (i = 750; i > 0; i--);      /* delay 1 ms */
}


float temp_decimal (unsigned char temp_tens, unsigned char temp_frac)
{
    float frac;
    float tens;

    if(temp_frac == '00')
        frac = 0.00;

    else if(temp_frac == '40')
            frac = 0.25;

    else if(temp_frac == '80')
            frac =  0.50;

    else
            frac = 0.75;

   tens = (float)temp_tens + frac;

return tens;
}


void read_everything (void){
    I2C1_Read(SLAVE_ADDR, 0, &sec); /* read second counter */
    I2C1_Read(SLAVE_ADDR, 1, &min); /* read min counter */
    I2C1_Read(SLAVE_ADDR, 2, &hrs); /* read hrs counter */
    I2C1_Read(SLAVE_ADDR, 3, &day); /* read day counter */
    I2C1_Read(SLAVE_ADDR, 4, &date); /* read date counter */
    I2C1_Read(SLAVE_ADDR, 5, &month); /* read month counter */
    I2C1_Read(SLAVE_ADDR, 6, &year); /* read year counter */
    I2C1_Read(SLAVE_ADDR, 17, &temp_msb); /* read temp_msb counter */
    I2C1_Read(SLAVE_ADDR, 18, &temp_lsb); /* read temp_lsb counter */

    printf("%02x/%02x/%02x DAY: %02x Time: %02x:%02x:%02x\n", year, month, date, day, hrs, min, sec);

}

void print_date_month_year(void){
    I2C1_Read(SLAVE_ADDR, 3, &day); /* read day counter */
    I2C1_Read(SLAVE_ADDR, 4, &date); /* read date counter */
       I2C1_Read(SLAVE_ADDR, 5, &month); /* read month counter */
       I2C1_Read(SLAVE_ADDR, 6, &year); /* read year counter */
       printf("Day/Date/Month/Year:\n");
       printf("%02x/%02x/%02x/%02x\n\n",day, date, month, year);

}


void print_hours_minutes_seconds(void){

    I2C1_Read(SLAVE_ADDR, 0, &sec); /* read second counter */
        I2C1_Read(SLAVE_ADDR, 1, &min); /* read min counter */
        I2C1_Read(SLAVE_ADDR, 2, &hrs); /* read hrs counter */
        printf(" Hours:minutes:seconds \n");
        printf("  %02x: %02x: %02x\n\n",hrs, min, sec);
}



void print_temp(void){

    I2C1_Read(SLAVE_ADDR, 17, &temp_msb); /* read temp_msb counter */
    I2C1_Read(SLAVE_ADDR, 18, &temp_lsb); /* read temp_lsb counter */

        printf("Temp: %0.2f C\n\n",temp_decimal(temp_msb,temp_lsb));
}
