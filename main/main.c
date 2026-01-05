/* Becky Perkins
Jan 5th, 2026
blah blah blah description */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"

#define LED_PIN            10
#define TIMER_PERIOD_MS    1000
#define STOP_TIME_MS       5000
#define RESTART_TIME_MS    2000

TimerHandle_t led_timer;
TimerHandle_t control_timer;

// LED timer callback
void led_timer_callback(TimerHandle_t xTimer)
{
    static int led_state = 0;
    led_state = !led_state;
    gpio_set_level(LED_PIN, led_state);
    printf("LED Timer callback | LED state = %d\n", led_state);
}

// Control timer callback
void control_timer_callback(TimerHandle_t xTimer)
{
    static int state = 0;

    if (state == 0) {
        printf("Stopping LED timer...\n");
        xTimerStop(led_timer, 0);

        // Change control timer to restart delay
        xTimerChangePeriod(control_timer, pdMS_TO_TICKS(RESTART_TIME_MS), 0);
        state = 1;
    } else {
        printf("Restarting LED timer...\n");
        xTimerStart(led_timer, 0);

        // Stop control timer after restart
        xTimerStop(control_timer, 0);
    }
}

void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Create LED timer (periodic)
    led_timer = xTimerCreate("LED_Timer", pdMS_TO_TICKS(TIMER_PERIOD_MS), pdTRUE, NULL, led_timer_callback);

    // Create control timer (one-shot)
    control_timer = xTimerCreate("Control_Timer", pdMS_TO_TICKS(STOP_TIME_MS), pdFALSE, NULL, control_timer_callback);

    if (led_timer && control_timer){
        printf("Starting LED timer...\n");
        xTimerStart(led_timer, 0);

        printf("Starting control timer...\n");
        xTimerStart(control_timer, 0);
    } else {
        printf("Timer creation failed!\n");
    }
}
