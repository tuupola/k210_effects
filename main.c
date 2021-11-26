/*

MIT No Attribution

Copyright (c) 2021 Mika Tuupola

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-cut-

SPDX-License-Identifier: MIT-0

*/

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#include <bsp.h>
#include <timer.h>
#include <sysctl.h>
#include <plic.h>

#include <hagl_hal.h>
#include <bitmap.h>
#include <hagl.h>
#include <font6x9.h>
#include <fps.h>
#include <aps.h>

#include "metaballs.h"
#include "plasma.h"
#include "rotozoom.h"
#include "deform.h"

static uint8_t effect = 0;
volatile bool fps_flag = false;
volatile bool switch_flag = true;
static float effect_fps;
static float display_bps;

static bitmap_t *bb;
wchar_t message[32];

static char demo[4][32] = {
    "METABALLS",
    "PLASMA",
    "ROTOZOOM",
    "DEFORM",
};

void switch_timer_callback(void) {
    switch_flag = true;
}

void fps_timer_callback(void) {
    fps_flag = true;
}

void static inline switch_demo() {
    switch_flag = false;

    switch(effect) {
    case 0:
        printf("Closing metaballs.\n");
        //metaballs_close();
        break;
    case 1:
        printf("Closing plasma.\n");
        plasma_close();
        break;
    case 2:
        printf("Closing rotozoom.\n");
        //rotozoom_close();
        break;
    case 3:
        printf("Closing deform.\n");
        deform_close();
        break;
    }

    effect = (effect + 1) % 4;

    switch(effect) {
    case 0:
        printf("Initialising metaballs.\n");
        metaballs_init();
        break;
    case 1:
        printf("Initialising plasma.\n");
        plasma_init();
        break;
    case 2:
        printf("Initialising rotozoom.\n");
        rotozoom_init();
        break;
    case 3:
        printf("Initialising deform.\n");
        deform_init();
        break;
    }
}

void static inline show_fps() {
    color_t green = hagl_color(0, 255, 0);

    fps_flag = 0;

    /* Set clip window to full screen so we can display the messages. */
    hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

    /* Print the message on top left corner. */
    swprintf(message, sizeof(message), L"%s    ", demo[effect]);
    hagl_put_text(message, 4, 4, green, font6x9);

    /* Print the message on lower left corner. */
    swprintf(message, sizeof(message), L"%.*f FPS      ", 0, effect_fps);
    hagl_put_text(message, 4, DISPLAY_HEIGHT - 14, green, font6x9);

    /* Print the message on lower right corner. */
    swprintf(message, sizeof(message), L"%.*f KBPS     ", 0, display_bps / 1000);
    hagl_put_text(message, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT - 14, green, font6x9);

    /* Set clip window back to smaller so effects do not mess the messages. */
    hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);
}

int main()
{
    size_t bytes = 0;

    plic_init();
    sysctl_enable_irq();

    /* Update displayed FPS counter every 250 ms. */
    timer_init(TIMER_DEVICE_0);
    timer_set_interval(TIMER_DEVICE_0, TIMER_CHANNEL_0, 25e7);
    timer_set_irq(TIMER_DEVICE_0, TIMER_CHANNEL_0, fps_timer_callback, 1);
    timer_set_enable(TIMER_DEVICE_0, TIMER_CHANNEL_0, 1);

    /* Change demo every 10 seconds. */
    timer_init(TIMER_DEVICE_1);
    timer_set_interval(TIMER_DEVICE_1, TIMER_CHANNEL_0, 1e10);
    timer_set_irq(TIMER_DEVICE_1, TIMER_CHANNEL_0, switch_timer_callback, 1);
    timer_set_enable(TIMER_DEVICE_1, TIMER_CHANNEL_0, 1);

    hagl_init();
    hagl_clear_screen();
    hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);

    while (1) {
        switch(effect) {
        case 0:
            metaballs_animate();
            metaballs_render();
            break;
        case 1:
            plasma_animate();
            plasma_render();
            break;
        case 2:
            rotozoom_animate();
            rotozoom_render();
            break;
        case 3:
            deform_animate();
            deform_render();
            break;
        }

        /* Update the displayed fps if requested. */
        if (fps_flag) {
            show_fps();
        }

        /* Flush back buffer contents to display. NOP if single buffering. */
        bytes = hagl_flush();

        display_bps = aps(bytes);
        effect_fps = fps();

        /* Print the message in console and switch to next demo. */
        if (switch_flag) {
            printf("%s at %d fps / %d kBps\r\n", demo[effect], (uint32_t)effect_fps, (uint32_t)(display_bps / 1000));
            switch_demo();
            aps(APS_RESET);
        }
    };

    return 0;
}
