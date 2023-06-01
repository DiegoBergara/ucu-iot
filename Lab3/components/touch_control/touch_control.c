#include <stdio.h>
#include "touch_control.h"
#include "driver/touch_pad.h"

#define TOUCH_BUTTON_NUM    5

static const touch_pad_t buttons[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM1, // volume up
    TOUCH_PAD_NUM2, // play/pause
    TOUCH_PAD_NUM3, // volume down
    TOUCH_PAD_NUM4, // guard sensor
    TOUCH_PAD_NUM5, // record
};

void touch_init(void){

    touch_pad_init();
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_pad_config(buttons[i]);
    }

    /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };

    touch_pad_denoise_set_config(&denoise);
    touch_pad_denoise_enable();

    /* Enable touch sensor clock. Work mode is "timer trigger". */
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_fsm_start();

}

bool play_pause_button_pressed(void){
    uint16_t touch_value;
    touch_pad_read_raw_data(buttons[1], &touch_value);

    return touch_value > 30000;
}

bool record_button_pressed(void){
    uint16_t touch_value;
    touch_pad_read_raw_data(buttons[4], &touch_value);

    return touch_value > 30000;
}

bool vol_up_button_pressed(void){
    uint16_t touch_value;
    touch_pad_read_raw_data(buttons[0], &touch_value);

    return touch_value > 30000;
}

bool vol_down_button_pressed(void){
    uint16_t touch_value;
    touch_pad_read_raw_data(buttons[2], &touch_value);

    return touch_value > 30000;
}

bool guard_button_pressed(void){
    uint16_t touch_value;
    touch_pad_read_raw_data(buttons[3], &touch_value);

    return touch_value > 30000;
}

