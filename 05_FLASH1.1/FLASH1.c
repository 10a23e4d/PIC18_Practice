#include "FLASH1.h"

/*
// ABが出力される（正常）
void main()
{
   int8 get_data;

   fprintf(PORT1, "==Program Start==\r\n");

   fprintf(PORT1, "Flash Setting Starting...\r\n");
   flash_setting();
   fprintf(PORT1, "Flash Setting done.\r\n");

   fprintf(PORT1, "Status Register Flash1G Starting...\r\n");
   status_register_flash1g();
   fprintf(PORT1, "Status Register Flash1G done.\r\n");

   fprintf(PORT1, "Read ID Flash1G Starting...\r\n");
   read_id_flash1g();
   fprintf(PORT1, "Read ID Flash1G done.\r\n");

   fprintf(PORT1, "Sector Erase Flash1G Starting...\r\n");
   sector_erase_flash1g(0000);
   fprintf(PORT1, "Sector Erase Flash1G done.\r\n");

   fprintf(PORT1, "Write Data Flash1G Starting...\r\n");
   write_data_byte_flash1g(0x00, 0xAB);
   fprintf(PORT1, "Write Data Flash1G done.\r\n");

   fprintf(PORT1, "Read Data Flash1G Starting...\r\n");
   get_data = read_data_byte_flash1g(0x0000);
   fprintf(PORT1, "Read Data(Hex): %X\r\n", get_data);
   fprintf(PORT1, "Read Data Flash1G done.\r\n");

}
*/

/*

// NOIHSが表示されるもの (正常）
void main()
{
   // ここに書き込みたい文字列を指定してください（5文字程度推奨）
   char write_str[] = "NOIHS";

   // 定義した文字列を表示
   fprintf(PORT1, "String to Write: %s\r\n", write_str);

   // 文字列の長さを計算（終端のヌル文字分を除く）
   int8 data_len = sizeof(write_str) - 1;

   // 読み出したデータを格納するバッファ（少し大きめに確保）
   int8 read_buf[20];
   int8 i;

   fprintf(PORT1, "==Program Start==\r\n");

   fprintf(PORT1, "Flash Setting Starting...\r\n");
   flash_setting();
   fprintf(PORT1, "Flash Setting done.\r\n");

   fprintf(PORT1, "Status Register Flash1G Starting...\r\n");
   status_register_flash1g();
   fprintf(PORT1, "Status Register Flash1G done.\r\n");

   fprintf(PORT1, "Read ID Flash1G Starting...\r\n");
   read_id_flash1g();
   fprintf(PORT1, "Read ID Flash1G done.\r\n");

   fprintf(PORT1, "Sector Erase Flash1G Starting...\r\n");
   // アドレス 0x0000 を含むセクタを消去
   sector_erase_flash1g(0x0000);
   fprintf(PORT1, "Sector Erase Flash1G done.\r\n");

   fprintf(PORT1, "Write Data Flash1G Starting...\r\n");

   // 文字列の書き込み
   fprintf(PORT1, "Writing String: %s (Length: %u)\r\n", write_str, data_len);
   // (アドレス, 長さ, データポインタ)
   write_data_bytes_flash1g(0x0000, data_len, (int8*)write_str);

   fprintf(PORT1, "Write Data Flash1G done.\r\n");

   fprintf(PORT1, "Read Data Flash1G Starting...\r\n");

   // 読み出しバッファを一旦0(ヌル文字)でクリアしておく
   for(i = 0; i < sizeof(read_buf); i++) {
      read_buf[i] = 0;
   }

   // データの読み出し
   // (アドレス, 長さ, データ格納先ポインタ)
   read_data_bytes_flash1g(0x0000, data_len, read_buf);

   // 結果の表示
   // %s で文字列として表示
   fprintf(PORT1, "Read Data(String): %s\r\n", read_buf);

   // 1バイトずつHEXで表示して詳細確認
   fprintf(PORT1, "Read Data(Hex): ");
   for(i = 0; i < data_len; i++) {
      fprintf(PORT1, "%02X ", read_buf[i]);
   }
   fprintf(PORT1, "\r\n");

   fprintf(PORT1, "Read Data Flash1G done.\r\n");
}
*/

/*
// キーボード入力を受け付け (正常)
void main()
{
   // 入力用と読み出し用のバッファ
   char write_str[21];
   int8 read_buf[21];

   int8 data_len;
   int8 i;
   char c;

   fprintf(PORT1, "==Program Start==\r\n");

   fprintf(PORT1, "Flash Setting Starting...\r\n");
   flash_setting();
   fprintf(PORT1, "Flash Setting done.\r\n");

   fprintf(PORT1, "Status Register Flash1G Starting...\r\n");
   status_register_flash1g();
   fprintf(PORT1, "Status Register Flash1G done.\r\n");

   fprintf(PORT1, "Read ID Flash1G Starting...\r\n");
   if(read_id_flash1g()) {
      fprintf(PORT1, "Flash ID Check OK.\r\n");
   } else {
      fprintf(PORT1, "Flash ID Check FAILED. (Check Connections)\r\n");
   }
   fprintf(PORT1, "Read ID Flash1G done.\r\n");

   // メインループ：何度も入力→書き込み→読み出しを行えるようにする
   while(TRUE)
   {
      fprintf(PORT1, "\r\n================================\r\n");
      fprintf(PORT1, "Please enter text (max 20 chars) and press ENTER:\r\n> ");

      // --- 文字列入力処理 (エコーバック付き) ---
      data_len = 0;
      while(TRUE)
      {
         c = fgetc(PORT1); // UART1から1文字受信 (ブロッキング)

         if(c == '\r') // Enterキーが押されたら終了
         {
             break;
         }
         else if(c == 0x08 || c == 0x7F) // バックスペースキーの処理
         {
             if(data_len > 0)
             {
                 data_len--;
                 fputc(0x08, PORT1); // カーソルを戻す
                 fputc(' ', PORT1);  // 空白で文字を消す
                 fputc(0x08, PORT1); // 再度カーソルを戻す
             }
         }
         else if(data_len < 20) // 通常の文字入力 (バッファ上限まで)
         {
             write_str[data_len] = c;
             data_len++;
             fputc(c, PORT1); // 入力した文字を画面に表示 (エコーバック)
         }
      }

      write_str[data_len] = '\0'; // 文字列の終端処理
      fprintf(PORT1, "\r\n"); // 改行

      // 何も入力されなかった場合はスキップ
      if (data_len == 0) {
          fprintf(PORT1, "No text entered. Try again.\r\n");
          continue;
      }

      fprintf(PORT1, "Input received: %s (Length: %u)\r\n", write_str, data_len);

      // --- セクタ消去 ---
      // 書き込み前に対象セクタ(ここでは0x0000を含むセクタ)を消去する必要があります
      fprintf(PORT1, "Sector Erase Flash1G (Addr: 0x0000)...\r\n");
      sector_erase_flash1g(0x0000);
      fprintf(PORT1, "Sector Erase done.\r\n");

      // --- 書き込み ---
      fprintf(PORT1, "Writing Data...\r\n");
      // (アドレス, 長さ, データポインタ)
      write_data_bytes_flash1g(0x0000, data_len, (int8*)write_str);
      fprintf(PORT1, "Write done.\r\n");

      // --- 読み出し検証 ---
      fprintf(PORT1, "Reading Data...\r\n");

      // 読み出しバッファをクリア
      for(i = 0; i < sizeof(read_buf); i++) read_buf[i] = 0;

      // データの読み出し
      read_data_bytes_flash1g(0x0000, data_len, read_buf);

      // 結果の表示
      fprintf(PORT1, "Read Result(String): %s\r\n", read_buf);

      // 詳細なHEX表示
      fprintf(PORT1, "Read Result(Hex): ");
      for(i = 0; i < data_len; i++) {
         fprintf(PORT1, "%02X ", read_buf[i]);
      }
      fprintf(PORT1, "\r\n");

      // 一致確認
      // 簡易的なチェック（先頭文字と長さが合っているかなど）
      // 必要であれば strcmp などで厳密に比較可能
   }
}
*/


// ADC模擬コード ()
// --- ADC値取得用関数（プロトタイプ） ---
unsigned int16 get_adc_sample(void);
#include <stdlib.h>

void main()
{
   // 12bit ADCの値（0～4095）
   unsigned int16 adc_value;
   unsigned int16 read_value;

   // Flash書き込み用バッファ（2バイト）
   int8 data_buf[2];

   // 書き込み先アドレス（練習用として固定）
   int32 flash_addr = 0x0000;

   fprintf(PORT1, "== ADC Data Logging Test Program ==\r\n");

   // --- 初期化処理 ---
   fprintf(PORT1, "Initializing Flash...\r\n");
   flash_setting();
   status_register_flash1g();

   if(read_id_flash1g()) {
      fprintf(PORT1, "Flash Ready.\r\n");
   } else {
      fprintf(PORT1, "Flash Error! Check connections.\r\n");
   }

   // --- ADCの初期化（必要な場合） ---
   // setup_adc(ADC_CLOCK_INTERNAL);
   // setup_adc_ports(sAN0); // 例

   fprintf(PORT1, "\r\n=== Ready ===\r\n");
   fprintf(PORT1, "Press [ENTER] to capture ADC value and save to Flash.\r\n");

   while(TRUE)
   {
      // キー入力待ち（Enterキーが押されるまでブロック）
      char c = fgetc(PORT1);
      if(c != '\r') continue;

      fprintf(PORT1, "\r\n[Action] Sampling Start...\r\n");

      // 1. ADC値の取得
      // 本番ではこの関数の中身で実際のADCを読みに行きます
      adc_value = get_adc_sample();

      fprintf(PORT1, "ADC Value: %lu (0x%04lX)\r\n", adc_value, adc_value);

      // 2. データの分割 (バイナリ化)
      // 上位8bit
      data_buf[0] = (int8)((adc_value >> 8) & 0xFF);
      // 下位8bit
      data_buf[1] = (int8)(adc_value & 0xFF);

      // 3. Flashへの書き込み
      // 注意: 上書きできないため、練習用に毎回セクタを消去してから書き込みます
      // 実運用でログを取る場合は、アドレスをずらしながら追記していきます
      sector_erase_flash1g(flash_addr);

      // 書き込み実行
      write_data_bytes_flash1g(flash_addr, 2, data_buf);
      fprintf(PORT1, "Saved to Flash Addr 0x%04lX.\r\n", flash_addr);

      // 4. 検証（読み出して確認）
      // バッファをクリア
      data_buf[0] = 0;
      data_buf[1] = 0;

      read_data_bytes_flash1g(flash_addr, 2, data_buf);

      // 復元
      read_value = ((unsigned int16)data_buf[0] << 8) | ((unsigned int16)data_buf[1] & 0xFF);

      // 結果表示
      fprintf(PORT1, "Read Back: %lu (0x%04lX) -> ", read_value, read_value);

      if(adc_value == read_value) {
         fprintf(PORT1, "OK [Match]\r\n");
      } else {
         fprintf(PORT1, "NG [Mismatch]\r\n");
      }

      fprintf(PORT1, "> Press [ENTER] to next sample...\r\n");
   }
}

// --- ADC値取得関数 ---
// 現在はテスト用にダミー値を返します。
// 本番ではここを書き換えて実際のADC値を返してください。
unsigned int16 get_adc_sample(void)
{
   static unsigned int16 dummy_value = 0;
   unsigned int16 val;

   // === 本番用コード例 (内蔵ADCの場合) ===
   // set_adc_channel(0);
   // delay_us(10);
   // val = read_adc();
   // return val;

   // === 本番用コード例 (外部ADC LTC2452 / SPIの場合) ===
   // output_low(ADC_CS_PIN);
   // val = spi_xfer(ADC_SPI_PORT, 0x00) << 8; // 上位バイト受信
   // val |= spi_xfer(ADC_SPI_PORT, 0x00);     // 下位バイト受信
   // output_high(ADC_CS_PIN);
   // return val & 0x0FFF; // 12bitマスクなど

   // === テスト用ダミーコード ===
   // --- ADC値取得関数 ---
   // 12bit範囲(0-4095)の疑似乱数を返します
   // rand() は 0～32767 の範囲の整数を返します (CCS Cの場合)
   // これを 0x0FFF (4095) でマスクすることで、下位12ビットだけを取り出します。
   // これにより 0 ～ 4095 の範囲のランダムな値が得られます。

   return (unsigned int16)(rand() & 0x0FFF);
}