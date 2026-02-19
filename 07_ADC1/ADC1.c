#include "ADC1.h"

void main()
{
   // 変数宣言
   int16 adc_result; // 12bit ADCの結果を格納する変数 (0-4095)

   // --- 初期化処理 ---

   // 1. ADCポートの設定
   // 50番ピン(RD6)はアナログ入力に対応していないため、ここでは例として
   // PIN_A0 (AN0) をアナログ入力として設定します。
   // 使用するピンに合わせて sAN0 | sAN1 などを指定してください。
   // VSS_VDD: 電圧リファレンスを0V(VSS)と3.3V(VDD)に設定
   setup_adc_ports(sAN4 | VSS_VDD);

   // 2. ADCクロックの設定
   // 16MHz動作時、十分なTAD時間を確保するための設定
   setup_adc(ADC_CLOCK_DIV_32 | ADC_TAD_MUL_16);

   // ピンの出力設定などが必要な場合はここに追加
   // output_drive(PIN_xx);

   // 無限ループ
   while(TRUE)
   {
      // 3. チャンネル選択
      // 読み取りたいチャンネルを指定します (0 = AN0 = PIN_A0)
      set_adc_channel(4);

      // 4. アクイジション遅延
      // チャンネル切り替え後、電圧が安定するまで少し待ちます
      delay_us(20);

      // 5. ADC読み取り
      // 結果を変数に格納します
      adc_result = read_adc();

      // --- 確認用 ---
      // 読み取った値をUART1(PORT1)から出力して確認できます
      // 不要であれば削除してください
      #ifdef DEBUG_PORT
         fprintf(PORT1, "ADC Value: %lu\r\n", adc_result);
      #endif

      delay_ms(100); // 次の読み取りまで少し待機
   }
}