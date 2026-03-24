#include "main.h"

// 外部発振子(16MHz)を使用するためのヒューズ設定
#fuses HS

// UARTの設定: 3番ピン(PIN_G0)をTXとして使用、ボーレート9600bps
#use rs232(baud=9600, parity=N, xmit=PIN_G0, bits=8, stream=PC, FORCE_SW, ERRORS)

// ピンの割り当て定義
#define LED1   PIN_A5
#define LED2   PIN_A1

void main()
{
    // アナログ入力機能をすべて無効化し、すべてのピンをデジタルI/Oとして設定する
    setup_adc_ports(NO_ANALOGS);

    // メインループ
    while(TRUE)
    {
        // LED1(PIN_A5)とLED2(PIN_A1)をON (HIGH)
        output_high(LED1);
        output_high(LED2);

        // 状態をUARTで送信
        fprintf(PC, "LED ON\r\n");

        // 1秒待機
        delay_ms(1000);

        // LED1(PIN_A5)とLED2(PIN_A1)をOFF (LOW)
        output_low(LED1);
        output_low(LED2);

        // 状態をUARTで送信
        fprintf(PC, "LED OFF\r\n");

        // 1秒待機
        delay_ms(1000);
    }
}