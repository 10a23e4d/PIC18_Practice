/////LED1/////
//#include <18F67J94.h>
#include "TEMP.h"

void main() {
    // 全ピンをデジタルモードに設定（アナログ機能をオフ）
    setup_adc_ports(NO_ANALOGS);

    // 各ピンを出力設定にし、High(3.3V)を出力
    // CS(A2)をHighにすることで、接続されているスレーブICを非アクティブ(Hi-Z)にします
    output_high(PIN_A2); // CS
    output_high(PIN_A3); // CLK
    output_high(PIN_A0); // DI (MISO) -> これの電圧を測定してください
    output_high(PIN_A1); // DO (MOSI)

    while(TRUE) {
        // High出力を維持
        // もし短絡していれば、ここで電流が流れ続けます
    }
}