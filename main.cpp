#include <mbed.h>
#include "uLCD_4DGL.h"
#include <iostream>
#define sampleAmount 1000

uLCD_4DGL   uLCD(D1, D0, D2); 
InterruptIn button_up(D10);
InterruptIn button_down(D12);
InterruptIn button_select(D11);
AnalogOut   aout(D7);
AnalogIn    ain(A0);

Thread waveThread;
Thread sampleThread;
EventQueue waveQueue;
EventQueue sampleQueue;

int    slew = 1;
int    slew_mode = 3;
int    slew_table[4] = {8, 4, 2, 1};
int    slew_time = 0;
float  delta = 0;
bool   if_change = 1;
bool   if_generate = 0;
float  sample[1000];

void monitor_display_menu();
void monitor_display_wave();
void ISR1 ();
void ISR2 ();
void ISR3 ();
void wave_generate();
void wave_sample();

void monitor_display_menu()
{
    if (!if_change) return;
    if_change = 0;
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.textbackground_color(GREEN);
    uLCD.color(BLACK);
    uLCD.locate(0, 15);
    uLCD.printf(" Down ");
    uLCD.locate(7, 15);
    uLCD.printf("Select");
    uLCD.locate(14, 15);
    uLCD.printf(" Up "); 
    uLCD.text_width(3);
    uLCD.text_height(3);
    uLCD.color(WHITE);
    uLCD.textbackground_color(BLACK);
    uLCD.locate(0, 2);
    uLCD.printf("%.3f", 1.0 / slew_table[slew_mode]);

}

void monitor_display_wave()
{
    if (!if_generate) return;
    if_generate = 0;
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.text_width(3);
    uLCD.text_height(3);
    uLCD.color(RED);
    uLCD.textbackground_color(BLACK);
    uLCD.locate(0, 2);
    uLCD.printf("%.3f", 1.0 / slew_table[slew_mode]);
}

void ISR1 ()
{
    if (slew_mode == 3) return;
    slew_mode++;
    if_change = 1;
}

void ISR2 ()
{
    if (slew_mode == 0) return;
    slew_mode--;
    if_change = 1;
}

void ISR3 ()
{
    slew = slew_table[slew_mode];
    delta = 0.9f / (80 / slew);
    slew_time = 80 / slew;
    if_generate = 1;
    waveQueue.call(&wave_generate);
    sampleQueue.call(&wave_sample);
}

void wave_generate()
{
    while (1)
    {
        aout = 0;
        int i = 0;
        for (i = 1; i <= slew_time; i++)
        {
            aout = (i * delta);
            wait_us(500);
        }
        for (; i <= 240 - slew_time; i++)
        {
            aout = 0.9;
            wait_us(500);
        }
        for (i = 1; i <= slew_time; i++)
        {
            aout = 0.9 - (i * delta);
            wait_us(500);
        }
    }
}

void wave_sample()
{
    while (1)
    {
        for (int i = 0; i < sampleAmount; i++)
        {
            sample[i] = ain;
            ThisThread::sleep_for(1ms);
        }
        for (int i = 0; i < sampleAmount; i++)
        {
            printf("%f\r\n", sample[i]);
        }
        ThisThread::sleep_for(5000ms);
    }
}

int main()
{
    waveThread.start(callback(&waveQueue, &EventQueue::dispatch_forever));
    sampleThread.start(callback(&sampleQueue, &EventQueue::dispatch_forever));
    button_up.rise(&ISR1);
    button_down.rise(&ISR2);
    button_select.rise(&ISR3);
    while (1)
    {
        monitor_display_menu();
        monitor_display_wave();
    }
    return 0;
}