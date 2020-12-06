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
#include "board.h"

#include "main.h"
#include <lrr_usart.h>
#include <lrr_math.h>

#define ADC_RES 4096
#define V_REF 3.3
#define V_DIV_R1 100000
#define V_DIV_R2 3300

#define CURR_V_REF 5.0
// #define CURRENT_SENS_mVA 0.185 // +/- 5A
// #define CURRENT_SENS_mVA 0.100 // +/- 20A
#define CURRENT_SENS_mVA 0.066 // +/- 30A
//volatile float current_zero = 2.4755;
volatile float current_zero = 0;

#define ERROR_VOLT_VALUE 99.9
#define ERROR_CURR_VALUE 29.9

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

volatile int btn_reset_state = 0;
volatile int btn_disable_state = 0;
volatile int btn_select_state = 0;

volatile float current = 0;
volatile float voltage = 0;
volatile float max_measured_curr = 0;

volatile float absolute_current_max = 4.0;
volatile float current_max = 2.0;
volatile int speed_ms = 25;

int fuse_watchdog = 0;

volatile int fuse_on = 1;
volatile int fuse_blowed_notify = 0;

volatile int calibration_done = 0;
#define CALL_MEASURES 5
int cal_measure = CALL_MEASURES;

int cnt1 = 0;

void init_board(void)
{
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_4);

    cnt1 = __HAL_TIM_GET_COUNTER(&htim3);

    HAL_ADC_Start(&hadc1);

    mosfet_off();
    HAL_TIM_Base_Start_IT(&htim2);

    // wait until callibration is done
    while (!calibration_done);
}

void ctrl_led_on(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}

void ctrl_led_off(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

void on_led_on(void)
{
    HAL_GPIO_WritePin(ON_GPIO_Port, ON_Pin, GPIO_PIN_SET);
}

void on_led_off(void)
{
    HAL_GPIO_WritePin(ON_GPIO_Port, ON_Pin, GPIO_PIN_RESET);
}

void off_led_on(void)
{
    HAL_GPIO_WritePin(OFF_GPIO_Port, OFF_Pin, GPIO_PIN_SET);
}

void off_led_off(void)
{
    HAL_GPIO_WritePin(OFF_GPIO_Port, OFF_Pin, GPIO_PIN_RESET);
}

void disabled_led_on(void)
{
    HAL_GPIO_WritePin(DISABLED_GPIO_Port, DISABLED_Pin, GPIO_PIN_SET);
}

void disabled_led_off(void)
{
    HAL_GPIO_WritePin(DISABLED_GPIO_Port, DISABLED_Pin, GPIO_PIN_RESET);
}

void mosfet_on(void)
{
    HAL_GPIO_WritePin(DRV_MOSFET_GPIO_Port, DRV_MOSFET_Pin, GPIO_PIN_RESET);
}

void mosfet_off(void)
{
    HAL_GPIO_WritePin(DRV_MOSFET_GPIO_Port, DRV_MOSFET_Pin, GPIO_PIN_SET);
}

int get_encoder_dx(void)
{
    int cnt2 = __HAL_TIM_GET_COUNTER(&htim3);

    int diff = cnt1 - cnt2;

    if (diff > 2) {
        diff = 2;
    } else if (diff < -2) {
        diff = -2;
    }

    cnt1 = __HAL_TIM_GET_COUNTER(&htim3);
    return diff;
}

int btn_disable_released(void)
{
    if (btn_disable_state) {
        btn_disable_state = 0;
        return 1;
    }

    return 0;
}

int btn_reset_released(void)
{
    if (btn_reset_state) {
        btn_reset_state = 0;
        return 1;
    }

    return 0;
}

int btn_select_released(void)
{
    if (btn_select_state) {
        btn_select_state = 0;
        return 1;
    }

    return 0;
}

float get_voltage(void)
{
    return voltage;
}

float get_current(void)
{
    return current;
}

float get_max_current(void)
{
    return max_measured_curr;
}

void blow_fuse(void)
{
    mosfet_off();
    on_led_off();
    off_led_on();
    fuse_blowed_notify = 1;
}

void disable_fuse(void)
{
    fuse_on = 0;
    mosfet_on();
    on_led_off();
    disabled_led_on();
}

void enable_fuse(void)
{
    fuse_on = 1;
    max_measured_curr = 0;
    fuse_watchdog = speed_ms;
    mosfet_on();
    on_led_on();
    disabled_led_off();
    off_led_off();
}

int fuse_blowed()
{
    if (fuse_blowed_notify) {
        fuse_blowed_notify = 0;
        return 1;
    }

    return 0;
}

void set_absolute_max_current(float limit)
{
    absolute_current_max = limit;
}

void set_max_current(float limit)
{
    current_max = limit;
}

void set_fuse_speed(int ms)
{
    speed_ms = ms;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == DISABLE_Pin) {
        btn_disable_state = 1;
    }

    if (GPIO_Pin == SELECT_Pin) {
        btn_select_state = 1;
    }

    if (GPIO_Pin == RESET_Pin) {
        btn_reset_state = 1;
    }
}

static float v_err_map[][2] = 
{
    {0.0,        0.0},
    {0.3,        1.72},
    {2.55,       4.03},
    {9.7,       11.22},
    {26.1,      27.5},
    {47.5,      49.00},
    {64.2,      65.73},
    {84.3,      85.8},
    {100.0,     101.5},
    {200.0,     201.5}         
};

float calc_voltage(int raw)
{
    float v = V_REF * raw / ADC_RES;

    v = v * (V_DIV_R1 + V_DIV_R2) / V_DIV_R2;

    v = lrr_get_corrected(v_err_map, 
        sizeof(v_err_map) / sizeof(v_err_map[0]),
        v);
    return v;
}

float calc_current(int raw)
{
    float v = V_REF * raw / ADC_RES;

    if (!calibration_done) {
        current_zero += v;
        --cal_measure;

        if (cal_measure <= 0) {
            calibration_done = 1;
            current_zero /= CALL_MEASURES;
        } else {
            return 0;
        }
    }

    float dv = current_zero - v;
    float a = dv / CURRENT_SENS_mVA;

    if (a < 0.1) {
        a = 0.0;
    }
    return a;
}

uint16_t rawValues[2];

void int_conv_dma(DMA_HandleTypeDef *hdma)
{
    if (hdma->State == HAL_DMA_STATE_READY) {
        HAL_ADC_Stop_DMA(&hadc1);

        current = calc_current(rawValues[0]);
        voltage = calc_voltage(rawValues[1]);

        max_measured_curr = (current > max_measured_curr)? 
            current : max_measured_curr;

        if (fuse_on) {
            if (current >= absolute_current_max) {
                blow_fuse();
            } else if (current >= current_max) {
                --fuse_watchdog;
                if (fuse_watchdog <= 0) {
                    blow_fuse();
                }
            } else {
                fuse_watchdog = speed_ms;
            }
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)rawValues, 2);
    }
}