#include "ADC1.h"
#include <stdlib.h> // toupper等で使用

// --- Timer Global Variables ---
// 経過ミリ秒数をカウント (0 ～ 4,294,967,295 ms ≒ 約49日)
unsigned int32 total_milliseconds = 0;
int1 time_updated = 0;

// --- Timer Interrupt ---
#INT_TIMER0
void timer0_isr(void)
{
   // 1msごとの割り込み設定
   // 16MHz / 4(System) / 4(Prescaler) = 1MHz (1us/count)
   // 1ms = 1000us -> 1000 count
   // 65536 - 1000 = 64536
   set_timer0(64536);

   // ミリ秒をインクリメント
   total_milliseconds++;

   time_updated = 1;
}

// 状態管理用の定数
#define STATE_IDLE    0
#define STATE_LOGGING 1

// データパケットサイズ: Time(4byte) + ADC(2byte) + Padding(2byte) = 8バイト
// ※256(ページサイズ) ÷ 8 = 32 なので、ページ境界処理は不要です
#define PACKET_SIZE   8

void main()
{
    // --- 変数宣言 ---
    int16 adc_value;           // ADC測定値
    int32 current_addr = 0;    // 現在のFlash書き込み/読み出しアドレス
    int32 recorded_count = 0;  // 記録したデータ数（サンプル数）
    int8  state = STATE_IDLE;  // 現在の状態
    char  cmd;                 // 受信コマンド
    int8  flash_buf[PACKET_SIZE]; // Flash用バッファ

    // --- 初期化処理 ---
    fprintf(PORT1, "\r\n=== System Booting ===\r\n");

    // 1. ADC設定 (RA5/AN4)
    setup_adc_ports(sAN4 | VSS_VDD);
    setup_adc(ADC_CLOCK_DIV_32 | ADC_TAD_MUL_16);

    // 2. Timer設定 (1ms割り込みに変更)
    fprintf(PORT1, "Initializing Timer (1ms)...\r\n");
    // T0_DIV_4 に変更 (1us単位でカウントするため)
    setup_timer_0(T0_INTERNAL | T0_DIV_4);
    set_timer0(64536);
    enable_interrupts(INT_TIMER0);
    enable_interrupts(GLOBAL);

    // 3. Flash設定
    fprintf(PORT1, "Initializing Flash...\r\n");
    flash_setting();

    if(read_id_flash1g())
    {
        fprintf(PORT1, "Flash Connection OK.\r\n");
    }
    else
    {
        fprintf(PORT1, "Flash Connection FAILED. Check wiring.\r\n");
    }

    // 操作説明
    fprintf(PORT1, "\r\n=== Operation Menu ===\r\n");
    fprintf(PORT1, " [A] Start Logging (Erase & Record)\r\n");
    fprintf(PORT1, " [S] Stop Logging\r\n");
    fprintf(PORT1, " [D] Display Recorded Data (CSV)\r\n");
    fprintf(PORT1, "======================\r\n\r\n");

    // --- メインループ ---
    while(TRUE)
    {
        // --- 1. キー入力チェック (ノンブロッキング) ---
        if(kbhit(PORT1))
        {
            cmd = toupper(fgetc(PORT1));

            if(cmd == 'A')
            {
                // 記録開始処理
                fprintf(PORT1, "\r\n[CMD] Start Logging...\r\n");

                // セクタ0消去
                fprintf(PORT1, "Erasing Sector 0 (Please wait)...\r\n");
                sector_erase_flash1g(0x000000);

                // 変数リセット
                current_addr = 0x000000;
                recorded_count = 0;
                total_milliseconds = 0; // 時間リセット
                state = STATE_LOGGING;

                fprintf(PORT1, "Recording Started!\r\n");
            }
            else if(cmd == 'S')
            {
                // 停止処理
                if(state == STATE_LOGGING)
                {
                    state = STATE_IDLE;
                    fprintf(PORT1, "\r\n[CMD] Logging Stopped.\r\n");
                    fprintf(PORT1, "Total Samples: %lu\r\n", recorded_count);
                }
                else
                {
                    fprintf(PORT1, "\r\n[INFO] Not recording.\r\n");
                }
            }
            else if(cmd == 'D')
            {
                // 再生処理
                if(state == STATE_LOGGING)
                {
                    fprintf(PORT1, "\r\n[WARN] Please Stop (S) first.\r\n");
                }
                else if(recorded_count == 0)
                {
                    fprintf(PORT1, "\r\n[INFO] No data recorded.\r\n");
                }
                else
                {
                    fprintf(PORT1, "\r\n[CMD] Reading Flash Data...\r\n");
                    // Excel用CSVヘッダー: TIME(ms), COUNT, HEX, DEC, VOLT(V)
                    // 電圧の列を追加しました
                    fprintf(PORT1, "TIME(ms), COUNT, DATA_HEX, DATA_DEC, VOLT(V)\r\n");

                    int32 read_ptr = 0x000000;
                    int32 i;
                    float voltage; // 電圧計算用変数

                    for(i = 0; i < recorded_count; i++)
                    {
                        // 8バイト読み出し
                        read_data_bytes_flash1g(read_ptr, PACKET_SIZE, flash_buf);

                        // データの復元
                        // [0]-[3]: Time(32bit), [4]-[5]: ADC(16bit)
                        int32 r_time = make32(flash_buf[0], flash_buf[1], flash_buf[2], flash_buf[3]);
                        int16 r_val  = make16(flash_buf[4], flash_buf[5]);

                        // 電圧変換計算
                        // 電圧 = (ADC値 * 3.3V) / 4095
                        voltage = (float)r_val * 3.3 / 4095.0;

                        // CSV形式で表示 (%.3f は小数第3位まで表示という意味です)
                        fprintf(PORT1, "%lu, %lu, %03lX, %lu, %.3f\r\n",
                                r_time, i, r_val, r_val, voltage);

                        read_ptr += PACKET_SIZE;
                        delay_ms(1); // バッファ溢れ防止
                    }
                    fprintf(PORT1, "End of Data.\r\n");
                }
            }
        }

        // --- 2. 状態ごとの処理 ---
        if(state == STATE_LOGGING)
        {
            // ADC読み取り
            set_adc_channel(4); // AN4
            delay_us(20);
            adc_value = read_adc();

            // モニタ表示
            fprintf(PORT1, "Time:%lu ms | REC: %lu (Addr: 0x%04lX)\r\n",
                    total_milliseconds, adc_value, current_addr);

            // Flash書き込みデータ作成 (8バイト)
            // Time (4byte: 32bit)
            flash_buf[0] = make8(total_milliseconds, 3); // MSB
            flash_buf[1] = make8(total_milliseconds, 2);
            flash_buf[2] = make8(total_milliseconds, 1);
            flash_buf[3] = make8(total_milliseconds, 0); // LSB

            // ADC (2byte: 16bit)
            flash_buf[4] = make8(adc_value, 1);
            flash_buf[5] = make8(adc_value, 0);

            // Padding (2byte) - 将来の拡張用 or 予備
            flash_buf[6] = 0x00;
            flash_buf[7] = 0x00;

            // 書き込み実行 (8バイト単位なので256バイト境界も問題なし)
            write_data_bytes_flash1g(current_addr, PACKET_SIZE, flash_buf);

            current_addr += PACKET_SIZE;
            recorded_count++;

            // セーフティ
            if(current_addr >= 0xFFFF)
            {
                state = STATE_IDLE;
                fprintf(PORT1, "\r\n[WARN] Memory Sector Full. Auto Stopped.\r\n");
            }

            // サンプリング周期の調整
            delay_ms(100);
        }
        else
        {
            delay_ms(50);
        }
    }
}