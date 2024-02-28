/***********************************************************************
*
*  FILE        : aprx72n_sdram.c
*  DATE        : 2024-02-26
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include <r_smc_entry.h>
#include <r_dmaca_rx_if.h>
#include <stdio.h>
#include <string.h>

#include "hwtick.h"
#include "usb_cdc.h"
#include "led.h"

/**
 * @brief SDRAM アドレス
 */
#define SDRAM_ADDR (0x08000000UL)
/**
 * @brief SDRAMサイズ
 */
#define SDRAM_SIZE (32UL * 1024UL * 1024UL)

/**
 * @brief SDRAM アドレス
 */
#define SDRAM_PTR ((void*)((uintptr_t)(SDRAM_ADDR)))

/**
 * @brief RAM2アドレス
 */
#define RAM2_ADDR (0x00800000UL)
/**
 * @brief RAM2ポインタ
 */
#define RAM2_PTR ((void*)((uintptr_t)(RAM2_ADDR)))

/**
 * @brief 転送テストデータ長
 */
#define TEST_LENGTH (512 * 1024)
//#define TEST_LENGTH (128 * 1024)

void main(void);
static bool init_dmaca_peripheral(uint8_t ch, void (*callback)(void));
static void init_dmaca_request(dmaca_transfer_data_cfg_t *pcfg, void *src, void *dst, uint32_t length);
static void dmaca_int_callback_ram_to_sdram(void);
static void dmaca_int_callback_sdram_to_ram(void);
static void proc_transfer_test(void);
static bool start_dmaca_transfer(uint8_t ch, dmaca_transfer_data_cfg_t *pcfg);
static bool is_dmac_idle(uint8_t ch);
static void print_rate(const char *name, uint32_t transfer_time);

/**
 * DMACAチャンネル RAM -> SDRAM
 */
static const uint8_t s_dmaca_ch_ram_to_sdram = DMACA_CH0;

/**
 * DMACAチャンネル SDRAM -> RAM
 */
static const uint8_t s_dmaca_ch_sdram_to_ram = DMACA_CH1;
/**
 * @brief DMACA 転送設定 RAM -> SDRAM
 */
static dmaca_transfer_data_cfg_t s_dmaca_cfg_ram_to_sdram;
/**
 * @brief DMACA 転送設定 SDRAM -> RAM
 */
static dmaca_transfer_data_cfg_t s_dmaca_cfg_sdram_to_ram;
/**
 * @brief RAM -> SDRAM 転送開始時Tick
 */
static uint32_t s_start_tick_ram_to_sdram = 0;
/**
 * @brief RAM -> SDRAM 転送時間[ms]
 */
static uint32_t s_transfer_time_ram_to_sdram = 0;
/**
 * @brief SDRAM -> RAM 転送開始時Tick
 */
static uint32_t s_start_tick_sdram_to_ram = 0;
/**
 * @brief SDRAM -> RAM 転送時間[ms]
 */
static uint32_t s_transfer_time_sdram_to_ram = 0;

#define TRANSFER_TEST_STATE_IDLE (0)
#define TRANSFER_TEST_STATE_RAM_TO_SDRAM (1)
#define TRANSFER_TEST_STATE_SDRAM_TO_RAM (2)
/**
 * @brief 転送テストステート
 */
static uint8_t s_transfer_test_state;
/**
 * @brief アプリケーションのエントリポイント
 */
void main(void)
{
    uint8_t tmp[64];

    s_transfer_test_state = TRANSFER_TEST_STATE_IDLE;

    // DMAC初期化
    R_DMACA_Init();

    // SDRAM 初期化
    R_Config_BSC_InitializeSDRAM();

    // 1msec ハードウェアTICKカウンタスタート
    hwtick_init();
    led_init();
    usb_cdc_init();

    init_dmaca_peripheral(s_dmaca_ch_ram_to_sdram, dmaca_int_callback_ram_to_sdram);
    init_dmaca_peripheral(s_dmaca_ch_sdram_to_ram, dmaca_int_callback_sdram_to_ram);

    init_dmaca_request(&s_dmaca_cfg_ram_to_sdram, RAM2_PTR, SDRAM_PTR, TEST_LENGTH);
    init_dmaca_request(&s_dmaca_cfg_sdram_to_ram, SDRAM_PTR, RAM2_PTR, TEST_LENGTH);

    uint32_t prev = hwtick_get();
    while (1)
    {
        usb_cdc_update();

        uint32_t now = hwtick_get();
        if ((now - prev) >= 1000)
        {
            proc_transfer_test(); // 転送トリガをかける
            prev = now;
        }

        if (usb_cdc_get_DSR()) // 接続先がある？
        {
            int rx_len = usb_cdc_read(tmp, sizeof(tmp));
            if (rx_len > 0)
            {
                // TODO : something.
            }

            if (usb_cdc_get_CTS()) // 送信可？
            {
                if (s_transfer_time_ram_to_sdram != 0)
                {
                    print_rate("RAM->SDRAM", s_transfer_time_ram_to_sdram);
                    s_transfer_time_ram_to_sdram = 0; // 出力データクリア
                }
                if (s_transfer_time_sdram_to_ram != 0)
                {
                    print_rate("SDRAM->RAM", s_transfer_time_sdram_to_ram);
                    s_transfer_time_sdram_to_ram = 0; // 出力データクリア
                }
            }
        }
    }

    return ;
}

/**
 * @brief DMACペリフェラルを初期化する。
 *        基本的に失敗したらソフトウェア設計ミスである。
 * @param ch 使用するDMACチャンネル
 * @param callback INT割り込みを受け取るコールバック
 * @return 成功した場合にはtrue, 失敗した場合にはfalse.
 */
static bool init_dmaca_peripheral(uint8_t ch, void (*callback)(void))
{
    if (R_DMACA_Open(ch) != DMACA_SUCCESS)
    {
        return false;
    }

    if (R_DMACA_Int_Callback(ch, (void*)(callback)) != DMACA_SUCCESS)
    {
        return false;
    }

    return true;
}

/**
 * @brief DMAC転送要求を初期化する。
 *        ブロック転送モードで転送する想定。
 * @param pcfg 初期化するDMAC要求
 * @param src 転送元アドレス
 * @param dst 転送先アドレス
 * @param length 長さ(転送単位の正数倍にする必要がある。)
 */
static void init_dmaca_request(dmaca_transfer_data_cfg_t *pcfg, void *src, void *dst, uint32_t length)
{
#define DMAC_TRANSFER_BLOCK_SIZE (512)

    memset(pcfg, 0, sizeof(dmaca_transfer_data_cfg_t));
    pcfg->transfer_mode = DMACA_TRANSFER_MODE_BLOCK;
    pcfg->repeat_block_side = DMACA_REPEAT_BLOCK_DISABLE;
    pcfg->data_size = DMACA_DATA_SIZE_WORD; // 転送単位 ワード単位(16bit)
    pcfg->act_source = 0;

    pcfg->request_source = DMACA_TRANSFER_REQUEST_SOFTWARE;
    pcfg->dtie_request = DMACA_TRANSFER_END_INTERRUPT_ENABLE; // 転送完了割り込み許可
    pcfg->esie_request = DMACA_TRANSFER_ESCAPE_END_INTERRUPT_ENABLE; // エスケープ転送割り込み許可
    pcfg->rptie_request = DMACA_REPEAT_SIZE_END_INTERRUPT_DISABLE;
    pcfg->sarie_request = DMACA_SRC_ADDR_EXT_REP_AREA_OVER_INTERRUPT_DISABLE;
    pcfg->darie_request = DMACA_DES_ADDR_EXT_REP_AREA_OVER_INTERRUPT_DISABLE;
    pcfg->src_addr_mode = DMACA_SRC_ADDR_INCR; // 転送元アドレスを加算する
    pcfg->src_addr_repeat_area = DMACA_SRC_ADDR_EXT_REP_AREA_NONE;
    pcfg->des_addr_mode = DMACA_DES_ADDR_INCR; // 早々先アドレスを加算する
    pcfg->des_addr_repeat_area = DMACA_DES_ADDR_EXT_REP_AREA_NONE;
    pcfg->offset_value = 0;
    pcfg->interrupt_sel = DMACA_CLEAR_INTERRUPT_FLAG_BEGINNING_TRANSFER;
    pcfg->p_src_addr = src;
    pcfg->p_des_addr = dst;
    pcfg->transfer_count = length / DMAC_TRANSFER_BLOCK_SIZE; // 512byteブロック
    pcfg->block_size = DMAC_TRANSFER_BLOCK_SIZE;

    return ;
}

/**
 * @brief DMACA 転送完了通知
 */
static void dmaca_int_callback_ram_to_sdram(void)
{
    dmaca_stat_t stat;
    if (R_DMACA_Control(s_dmaca_ch_ram_to_sdram, DMACA_CMD_STATUS_GET, &stat) != DMACA_SUCCESS)
    {
        return ;
    }
    if (stat.soft_req_stat // ソフトウェアによる転送要求が出ている？
            && (stat.dtif_stat || stat.esif_stat)) // 転送終了？
    {
        led_set(LED_LD1, false);
        s_transfer_time_ram_to_sdram = hwtick_get() - s_start_tick_ram_to_sdram;
        R_DMACA_Control(s_dmaca_ch_ram_to_sdram, DMACA_CMD_SOFT_REQ_CLR, NULL);
        R_DMACA_Int_Disable(s_dmaca_ch_ram_to_sdram);
        R_DMACA_Control(s_dmaca_ch_ram_to_sdram, DMACA_CMD_DISABLE, NULL);
    }
    return ;
}
/**
 * @brief DMACA 転送完了通知
 */
static void dmaca_int_callback_sdram_to_ram(void)
{
    dmaca_stat_t stat;
    if (R_DMACA_Control(s_dmaca_ch_sdram_to_ram, DMACA_CMD_STATUS_GET, &stat) != DMACA_SUCCESS)
    {
        return ;
    }
    if (stat.soft_req_stat // ソフトウェアによる転送要求が出ている？
            && (stat.dtif_stat || stat.esif_stat)) // 転送終了？
    {
        led_set(LED_LD2, false);
        s_transfer_time_sdram_to_ram = hwtick_get() - s_start_tick_sdram_to_ram;
        R_DMACA_Control(s_dmaca_ch_sdram_to_ram, DMACA_CMD_SOFT_REQ_CLR, NULL);
        R_DMACA_Int_Disable(s_dmaca_ch_sdram_to_ram);
        R_DMACA_Control(s_dmaca_ch_sdram_to_ram, DMACA_CMD_DISABLE, NULL);
    }

    return ;
}

/**
 * @brief 転送テストを実施する。
 */
static void proc_transfer_test(void)
{
    if (!is_dmac_idle(s_dmaca_ch_sdram_to_ram) // SDRAM -> RAM 転送中？
            || !is_dmac_idle(s_dmaca_ch_ram_to_sdram)) // RAM -> SDRAM 転送中？
    {
        return ;
    }

    switch (s_transfer_test_state)
    {
        case TRANSFER_TEST_STATE_IDLE:
        {
            uint32_t now = hwtick_get();
            s_transfer_time_ram_to_sdram = 0;
            if (start_dmaca_transfer(s_dmaca_ch_ram_to_sdram, &s_dmaca_cfg_ram_to_sdram))
            {
                s_start_tick_ram_to_sdram = now;
                led_set(LED_LD1, true);
                s_transfer_test_state = TRANSFER_TEST_STATE_RAM_TO_SDRAM;
            }

            break;
        }
        case TRANSFER_TEST_STATE_RAM_TO_SDRAM:
        {
            uint32_t now = hwtick_get();
            s_transfer_time_sdram_to_ram = 0;
            if (start_dmaca_transfer(s_dmaca_ch_sdram_to_ram, &s_dmaca_cfg_ram_to_sdram))
            {
                s_start_tick_sdram_to_ram = now;
                led_set(LED_LD2, true);
                s_transfer_test_state = TRANSFER_TEST_STATE_SDRAM_TO_RAM;
            }
            break;
        }
        case TRANSFER_TEST_STATE_SDRAM_TO_RAM:
        {
            s_transfer_test_state = TRANSFER_TEST_STATE_IDLE;
            break;
        }
        default:
        {
            s_transfer_test_state = TRANSFER_TEST_STATE_IDLE;
            break;
        }
    }

    return ;
}

/**
 * @brief 転送開始する。
 * @param ch DMACチャンネル
 * @param pcfg DMAC転送設定
 * @return 成功した場合にはtrue, 失敗した場合にはfalse.
 */
static bool start_dmaca_transfer(uint8_t ch, dmaca_transfer_data_cfg_t *pcfg)
{
    if (R_DMACA_Create(ch, pcfg) != DMACA_SUCCESS)
    {
        return false;
    }
    if (R_DMACA_Int_Enable(ch, 1) != DMACA_SUCCESS)
    {
        return false;
    }

    if (R_DMACA_Control(ch, DMACA_CMD_ENABLE, NULL) != DMACA_SUCCESS)
    {
        return false;
    }

    return (R_DMACA_Control(ch, DMACA_CMD_SOFT_REQ_NOT_CLR_REQ, NULL) == DMACA_SUCCESS);
}

/**
 * @brief chで指定されるDMACチャンネルが停止中かどうかを判定する。
 * @param ch DMACチャンネル
 * @return 停止中の場合にはtrue, 動作中の場合にはfalse.
 *         取得失敗時はfalseを返す。
 */
static bool is_dmac_idle(uint8_t ch)
{
    bool is_idle = false;
    dmaca_stat_t stat;

    if (R_DMACA_Control(s_dmaca_ch_sdram_to_ram, DMACA_CMD_STATUS_GET, &stat) == DMACA_SUCCESS)
    {
        is_idle = !stat.act_stat;
    }
    return is_idle;
}

/**
 * @brief 転送レートをUSB CDCに出力する。
 *        transfer_timeに0が指定された場合は何もしない。
 * @param name 転送レート名
 * @param transfer_time 転送時間[msec]
 */
static void print_rate(const char *name, uint32_t transfer_time)
{
    if (transfer_time == 0)
    {
        return ;
    }

    static char s_buf[128];

    float rate = TEST_LENGTH / (float)(transfer_time)
            * 1000.0f /* msec->sec */ / 1000000.0f /* byte -> MB */;

    int len = snprintf(s_buf, sizeof(s_buf), "%s %dmsec %.1fMB/sec\n",
                       name, transfer_time, rate);
    usb_cdc_write(s_buf, (uint16_t)(len));

    return ;
}
