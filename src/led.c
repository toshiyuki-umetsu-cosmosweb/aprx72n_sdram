/**
 * @file オンボードLEDインタフェース定義
 * @author Cosmosweb Co.,Ltd. 2024
 */
#include <platform.h>
#include "led.h"

#define LD1_PORT (PORT9.PODR.BIT.B5)
#define LD2_PORT (PORT9.PODR.BIT.B6)

/**
 * @brief LEDを初期化する。
 */
void led_init(void)
{
    LD1_PORT = 0u; // 消灯(H Active)
    LD2_PORT = 0u; // 消灯(H Active)

    return ;
}

/**
 * @brief 指定LEDの点灯状態を設定する。
 * @param no LED番号(LED_x 定義参照)
 * @param is_on 点灯させる場合にはtrue, 消灯させる場合にはfalse
 */
void led_set(int no, bool is_on)
{
    switch (no)
    {
        case LED_LD1:
        {
            LD1_PORT = (is_on) ? 1u : 0u;
            break;
        }
        case LED_LD2:
        {
            LD2_PORT = (is_on) ? 1u : 0u;
            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 * @brief 指定LEDが点灯中かどうかを取得する。
 * @param no LED番号(LED_x 定義参照)
 * @return 点灯中の場合にはtrue, 消灯中の場合にはfalse
 */
bool led_is_set(int no)
{
    bool ret;

    switch (no)
    {
        case LED_LD1:
        {
            ret = (LD1_PORT != 0u);
            break;
        }
        case LED_LD2:
        {
            ret = (LD2_PORT != 0u);
            break;
        }
        default:
        {
            ret = false;
            break;
        }
    }
    return ret;
}
