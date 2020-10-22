/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/touchio/TouchIn.h"
#include "py/runtime.h"

#include "driver/touch_pad.h"

static const touch_pad_t touch_pad[] = {
    TOUCH_PAD_NUM1,
    TOUCH_PAD_NUM2,
    TOUCH_PAD_NUM3,
    TOUCH_PAD_NUM4,
    TOUCH_PAD_NUM5,
    TOUCH_PAD_NUM6,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM8,
    TOUCH_PAD_NUM9,
    TOUCH_PAD_NUM10,
    TOUCH_PAD_NUM11,
    TOUCH_PAD_NUM12,
    TOUCH_PAD_NUM13,
    TOUCH_PAD_NUM14
};

static uint16_t get_raw_reading(touchio_touchin_obj_t *self) {
    uint32_t touch_value;
    touch_pad_read_raw_data(touch_pad[self->pin], &touch_value);
    return touch_value;
}

void common_hal_touchio_touchin_construct(touchio_touchin_obj_t* self,
        const mcu_pin_obj_t *pin) {
    if (!pin->has_touch) {
        mp_raise_ValueError(translate("Invalid pin"));
    }
    claim_pin(pin);

    touch_pad_init();
    touch_pad_config(touch_pad[pin->number]);

    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_fsm_start();

    // Initial values for pins will vary, depending on what peripherals the pins
    // share on-chip.

    // Set a "touched" threshold not too far above the initial value.
    // For simple finger touch, the values may vary as much as a factor of two,
    // but for touches using fruit or other objects, the difference is much less.

    self->pin = pin->number;
    self->threshold = get_raw_reading(self) + 100;
}

bool common_hal_touchio_touchin_deinited(touchio_touchin_obj_t* self) {
    return self->pin == 0xff;
}

void common_hal_touchio_touchin_deinit(touchio_touchin_obj_t* self) {
    if (common_hal_touchio_touchin_deinited(self)) {
        return;
    }

    //touch_pad_deinit();

    reset_pin_number(self->pin);
    self->pin = 0xff;
}

bool common_hal_touchio_touchin_get_value(touchio_touchin_obj_t *self) {
    uint16_t reading = get_raw_reading(self);
    return reading > self->threshold;
}

uint16_t common_hal_touchio_touchin_get_raw_value(touchio_touchin_obj_t *self) {
    return get_raw_reading(self);
}

uint16_t common_hal_touchio_touchin_get_threshold(touchio_touchin_obj_t *self) {
    return self->threshold;
}

void common_hal_touchio_touchin_set_threshold(touchio_touchin_obj_t *self, uint16_t new_threshold) {
    self->threshold = new_threshold;
}
