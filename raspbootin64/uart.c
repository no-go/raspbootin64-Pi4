/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "gpio.h"
#include "mbox.h"

/* PL011 UART registers (NOT UART3 ! It is UART0, now) */
#define UART3_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART3_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART3_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART3_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART3_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART3_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART3_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART3_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    *UART3_CR = 0;         // turn off UART

    /* set up clock for consistent divisor values */
    mbox[0] = 9*4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;           // UART clock
    mbox[6] = 3000000;     // 3Mhz
    mbox[7] = 0;           // clear turbo
    mbox[8] = MBOX_TAG_LAST;
    mbox_call(MBOX_CH_PROP);

    // map UART3 to GPIO pins
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // 111 to gpio14, gpio15 (mask/clear)
    r|=(4<<12)|(4<<15);    // 100 = alt0 (UART0)
    *GPFSEL1 = r;

    // remove pullup or pulldown
    r = *GPPUPPDN0;
    r &= ~((3<<28)|(3<<30));     // 11 to gpio14, gpio15 (mask/clear)
    r |= (0<<28)|(0<<30);        // 00 = no pullup or down
    *GPPUPPDN0 = r;

    *UART3_ICR = 0x7FF;    // clear interrupts
    *UART3_IBRD = 1;       // 115200 baud
    *UART3_FBRD = 40;
    *UART3_LCRH = (1 << 6) | (1 << 5) | (1 << 4); // Enable FIFO and 8 bit data transmission (1 stop bit, no parity)
    *UART3_CR = (1 << 9) | (1 << 8) | (1 << 0);     // enable Tx, Rx, FIFO
}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
    /* wait until we can send */
    do{asm volatile("nop");}while(*UART3_FR&0x20);
    /* write the character to the buffer */
    *UART3_DR=c;
}

/**
 * Receive a character
 */
char uart_getc() {
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(*UART3_FR&0x10);
    /* read it and return */
    return (char)(*UART3_DR);
}
