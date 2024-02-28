/**
 * @file オンボードLEDインタフェース
 * @author Cosmosweb Co.,Ltd. 2024
 */

#ifndef LED_H_
#define LED_H_

#include <stdbool.h>

#define LED_LD1 (0)
#define LED_LD2 (1)

void led_init(void);
void led_set(int no, bool is_on);
bool led_is_set(int no);


#endif /* LED_H_ */
