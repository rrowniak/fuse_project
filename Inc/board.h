/*
 * Copyright (c) 2020 Rafal Rowniak rrowniak.com
 * 
 * The author hereby grant you a non-exclusive, non-transferable,
 * free of charge right to copy, modify, merge, publish and distribute,
 * the Software for the sole purpose of performing non-commercial
 * scientific research, non-commercial education, or non-commercial 
 * artistic projects.
 * 
 * Any other use, in particular any use for commercial purposes,
 * is prohibited. This includes, without limitation, incorporation
 * in a commercial product, use in a commercial service, or production
 * of other artefacts for commercial purposes.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */
#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32f1xx_hal.h"

void init_board(void);

void ctrl_led_on(void);
void ctrl_led_off(void);

void on_led_on(void);
void on_led_off(void);

void off_led_on(void);
void off_led_off(void);

void disabled_led_on(void);
void disabled_led_off(void);

void mosfet_on(void);
void mosfet_off(void);

int get_encoder_dx(void);

int btn_disable_released(void);
int btn_reset_released(void);
int btn_select_released(void);

float get_voltage(void);
float get_current(void);
float get_max_current(void);

void disable_fuse(void);
void enable_fuse(void);

int fuse_blowed(void);

void set_absolute_max_current(float limit);
void set_max_current(float limit);
void set_fuse_speed(int ms);

void int_conv_dma(DMA_HandleTypeDef *hdma);

#endif // __BOARD_H__