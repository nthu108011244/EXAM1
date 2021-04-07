#include <mbed.h>
#include "uLCD_4DGL.h"
#include <iostream>

uLCD_4DGL   uLCD(D1, D0, D2); 
InterruptIn button_up(D10);
InterruptIn button_down(D12);
InterruptIn button_select(D11);
AnalogOut   aout(D7);
AnalogIn    ain(A0);

Thread waveThread;
EventQueue waveQueue;

float  slew = 1;
int    slew_mode = 3;
float  slew_table[4] = {0.125, 0.25, 0.5, 1};
float  delta = 0;
bool   if_change = 1;
bool   if_generate = 0;

void monitor_display_menu();
void monitor_display_wave();
void wave_generate();

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
    uLCD.printf("%4.4f", slew_table[slew_mode]);

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
    uLCD.printf("%4.4f", slew_table[slew_mode]);
}

void ISR1 ()
{
    if (slew_mode < 3) slew_mode++;
    if_change = 1;
}

void ISR2 ()
{
    if (slew_mode > 0) slew_mode--;
    if_change = 1;
}

void ISR3 ()
{
    slew = slew_table[slew_mode];
    delta = 0.9f / (80.0f * slew);
    if_generate = 1;
    waveQueue.call(&wave_generate);
}

void wave_generate()
{
    while (1)
    {
        aout = 0;
        for (float i = 0.0f; i < 0.9f * slew; i += delta)
        {
            aout = i;
            ThisThread::sleep_for(1ms);
        }
        for (float i = 0; i < 240 - (160 * slew); i++)
        {
            aout = 0.9;
            ThisThread::sleep_for(1ms);
        }
        for (float i = 0.9f; i > 0.0f * slew; i -= delta)
        {
            aout = i;
            ThisThread::sleep_for(1ms);
        }
    }
}

int main()
{
    waveThread.start(callback(&waveQueue, &EventQueue::dispatch_forever));
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