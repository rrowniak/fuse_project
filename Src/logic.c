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
#include "logic.h"
#include "board.h"
#include <lrr_hd44780.h>
#include <lrr_usart.h>
#include <lrr_flash.h>

#include <stdio.h>

extern UART_HandleTypeDef huart1;

int selection = 0;
#define BLOWED_MSG_TIME 20
int fuse_blowed_msg_wdg = 0;

int disabled = 0;
// extern WWDG_HandleTypeDef hwwdg;

typedef struct {
    int speed_ms;
    float max_curr;
    float abs_max_curr;
} config;

config cfg;

#define MAX_LINE 16
char lcd_line[17] = {0};

static void clean_line_buffer(void)
{
    for (int i = 0; i < MAX_LINE; ++i) {
        lcd_line[i] = ' ';
    }
    lcd_line[MAX_LINE] = '\0';
}

static void unit_2_line(float v, char unit, int latest_char)
{
    if (v < 0.1) {
        lcd_line[latest_char - 1] = '0';
    } else if (v < 10.0) {
        sprintf(&lcd_line[latest_char - 3], "%.1f", v);
    } else if (v < 100.0) {
        sprintf(&lcd_line[latest_char - 4], "%.1f", v);
    } else if (v < 1000.0) {
        sprintf(&lcd_line[latest_char - 3], "%.0f", v);
    } else if (v < 10000.0) {
        v /= 1000.0;
        sprintf(&lcd_line[latest_char - 4], "%.1fk", v);
    } else {
        v /= 1000.0;
        sprintf(&lcd_line[latest_char - 3], "%.0fk", v);
    }

    lcd_line[latest_char] = unit;
}

static void unit_2_line_int(unsigned int v, char unit, int latest_char)
{
    if (v < 10) {
        sprintf(&lcd_line[latest_char - 1], "%d", v);
    } else if (v < 100) {
        sprintf(&lcd_line[latest_char - 2], "%d", v);
    } else if (v < 1000) {
        sprintf(&lcd_line[latest_char - 3], "%d", v);
    }

    lcd_line[latest_char] = unit;
}

void update_board_cfg(void)
{
    set_absolute_max_current(cfg.abs_max_curr);
    set_max_current(cfg.max_curr);
    set_fuse_speed(cfg.speed_ms);
}

void save_cfg(void)
{
    flash_write((uint16_t*)&cfg, sizeof(config) / 2);
}

void load_cfg(void)
{
    flash_read((uint16_t*)&cfg, sizeof(config) / 2);
    // cfg.speed_ms = 25;
    // cfg.abs_max_curr = 15.0;
    // cfg.max_curr = 3.0;
}

void logic_init(void)
{
    // HAL_WWDG_Start(&hwwdg);
    // HAL_WWDG_Refresh(&hwwdg);

    ctrl_led_off();
    on_led_on();
    off_led_on();
    disabled_led_on();

    mosfet_off();

    usart_config(&huart1);

    // LCD init sequence
    lcd_init();
    lcd_on();
    lcd_clear();
    lcd_disable_cursor();

               //________________
    lcd_println("Welcome-setup...", 0);
    lcd_println("rrowniak.com", 1);

    init_board();

    HAL_Delay(500);

    // load config
    load_cfg();

    if (cfg.speed_ms < 0 || cfg.speed_ms > 1000
        || cfg.max_curr < 0 || cfg.max_curr > 30
        || cfg.abs_max_curr < 0 || cfg.abs_max_curr > 30) {

        cfg.speed_ms = 25;
        cfg.abs_max_curr = 10;
        cfg.max_curr = 5;

        save_cfg();
    }

    enable_fuse();

    ctrl_led_on();
}

int update_flash = -1;

void logic_update(void)
{
    // HAL_WWDG_Refresh(&hwwdg);
    HAL_Delay(100);

    if (btn_reset_released()) {
        enable_fuse();
        disabled = 0;
    }

    if (btn_select_released()) {
        ++selection;
        selection %= 3;
    }

    if (btn_disable_released()) {
        disable_fuse();
        disabled = 1;
    }

    if (fuse_blowed()) {
        fuse_blowed_msg_wdg = BLOWED_MSG_TIME;
    }

    int enc = get_encoder_dx();
    if (enc != 0) {
        switch (selection) {
            case 0: // speed
                cfg.speed_ms += enc;
                if (cfg.speed_ms < 0) {
                    cfg.speed_ms = 0;
                } else if (cfg.speed_ms > 1000) {
                    cfg.speed_ms = 1000;
                }
                break;
            case 1:
                cfg.max_curr += (0.1 * enc);
                if (cfg.max_curr < 0.5) {
                    cfg.max_curr = 0.5;
                } else if (cfg.max_curr > cfg.abs_max_curr) {
                    cfg.max_curr = cfg.abs_max_curr;
                }
                break;
            case 2:
                cfg.abs_max_curr += enc;
                if (cfg.abs_max_curr < 1) {
                    cfg.abs_max_curr = 1;
                } else if (cfg.abs_max_curr > 30) {
                    cfg.abs_max_curr = 30;
                }
                break;
        }

        update_board_cfg();
        update_flash = 10;
    }

    if (update_flash > 0) {
        --update_flash;
    }

    if (update_flash == 0) {
        save_cfg();
        update_flash = -1;
    }

    if (fuse_blowed_msg_wdg > 0) {
        float max = get_max_current();
                         //________________
        sprintf(lcd_line, "UPS! max: %.1fA", max);
        --fuse_blowed_msg_wdg;
    } else if (disabled) {
        sprintf(lcd_line, "FUSE DISABLED!!!");
    } else {
        // display encoder value
        sprintf(lcd_line, "%d %d %.1fA %.0fA", selection, 
            cfg.speed_ms, cfg.max_curr, cfg.abs_max_curr);
    }

    lcd_set_cursor(0, 0);
    lcd_printfln("%s", lcd_line);

    // display voltage, current and power
    float voltage = get_voltage();
    float current = get_current();
    int W = voltage * current;
    clean_line_buffer();
    unit_2_line(voltage, 'V', 4);
    unit_2_line_int(W, 'W', 9);
    unit_2_line(current, 'A', 15);
    lcd_set_cursor(0, 1);
    lcd_printfln("%s", lcd_line);
}

