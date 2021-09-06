/* Switch demo implementation using button and RGB LED
   
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <sdkconfig.h>

#include <iot_button.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_params.h> 

#include <app_reset.h>
#include <ws2812_led.h>
#include "app_priv.h"

/* This is the button that is used for toggling the power */
#define BUTTON_GPIO          21
#define BUTTON_GPIO_2         17
#define BUTTON_ACTIVE_LEVEL  0

/* This is the GPIO on which the power will be set */
#define OUTPUT_GPIO    14
#define OUTPUT_GPIO_2  27
static bool g_power_state = DEFAULT_POWER;
static bool g_power_state_nirmesh = DEFAULT_POWER;

/* These values correspoind to H,S,V = 120,100,10 */
#define DEFAULT_RED     0
#define DEFAULT_GREEN   25
#define DEFAULT_BLUE    0

#define WIFI_RESET_BUTTON_TIMEOUT       3
#define FACTORY_RESET_BUTTON_TIMEOUT    10

static void app_indicator_set(bool state)
{
    if (state) {
        ws2812_led_set_rgb(DEFAULT_RED, DEFAULT_GREEN, DEFAULT_BLUE);
    } else {
        ws2812_led_clear();
    }
}

static void app_indicator_init(void)
{
    ws2812_led_init();
    app_indicator_set(g_power_state);
}
static void push_btn_cb(void *arg)
{
    bool new_state = !g_power_state;
    app_driver_set_state(new_state);
    esp_rmaker_param_update_and_report(
            esp_rmaker_device_get_param_by_name(switch_device1, ESP_RMAKER_DEF_POWER_NAME),
            esp_rmaker_bool(new_state));
}

static void push_btn_cb1(void *arg)
{
    bool new_state_nirmesh = !g_power_state_nirmesh;
    app_driver_set_state2(new_state_nirmesh);
    esp_rmaker_param_update_and_report(
            esp_rmaker_device_get_param_by_name(switch_device2, ESP_RMAKER_DEF_POWER_NAME),
            esp_rmaker_bool(new_state_nirmesh));
}

static void set_power_state(bool target)
{
    gpio_set_level(OUTPUT_GPIO, target);
    app_indicator_set(target);
}

static void set_power_state2(bool target)
{
    gpio_set_level(OUTPUT_GPIO_2, target);
    app_indicator_set(target);
}


void app_driver_init2()
{
    button_handle_t btn_handle = iot_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LEVEL);
	button_handle_t btn_handle2 = iot_button_create(BUTTON_GPIO_2, BUTTON_ACTIVE_LEVEL);
	
    if (btn_handle) {
        /* Register a callback for a button tap (short press) event */
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_TAP, push_btn_cb, NULL);
        /* Register Wi-Fi reset and factory reset functionality on same button */
        app_reset_button_register(btn_handle, WIFI_RESET_BUTTON_TIMEOUT, FACTORY_RESET_BUTTON_TIMEOUT);
	}
	if (btn_handle2) {
        /* Register a callback for a button tap (short press) event */
        iot_button_set_evt_cb(btn_handle2, BUTTON_CB_TAP, push_btn_cb1, NULL);
        /* Register Wi-Fi reset and factory reset functionality on same button */
		
    }
	
    /* Configure power */
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 1,
    };
    io_conf.pin_bit_mask = (((uint64_t)1 << OUTPUT_GPIO) | ((uint64_t)1 << OUTPUT_GPIO_2));
    /* Configure the GPIO */
    gpio_config(&io_conf);
    app_indicator_init();
}

int IRAM_ATTR app_driver_set_state(bool state)
{
    
    g_power_state = state;
    set_power_state(g_power_state);
    
    return ESP_OK;
}

int IRAM_ATTR app_driver_set_state2(bool state)
{
    
    g_power_state_nirmesh = state;
    set_power_state2(g_power_state_nirmesh);
    
    return ESP_OK;
}

bool app_driver_get_state(void)
{
    return g_power_state;
}

bool app_driver_get_state2(void)
{
    return g_power_state_nirmesh;
}