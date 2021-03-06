/**
 * \file
 *
 * \brief XMEGA-A3BU BACnet application
 *
 */
#include <asf.h>
#include "bacnet/basic/sys/mstimer.h"
#include "rs485.h"
#include "led.h"
#include "adc-hdw.h"
#include "bacnet/datalink/dlmstp.h"
#include "bacnet.h"

static struct mstimer_callback_data_t BACnet_Callback;

/**
 * \brief Main function.
 *
 * Initializes the board, and runs the application in an infinite loop.
 */
int main(void)
{
    /* hardware initialization */
    sysclk_init();
    board_init();
    pmic_init();
    mstimer_init();
    rs485_init();
    led_init();
    adc_init();
#ifdef CONF_BOARD_ENABLE_RS485_XPLAINED
    // Enable display backlight
    gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);
#endif
    // Workaround for known issue: Enable RTC32 sysclk
    sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
    while (RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm) {
        // Wait for RTC32 sysclk to become stable
    }
    cpu_irq_enable();
    /* application initialization */
    rs485_baud_rate_set(38400);
    bacnet_init();
    /*  run forever - timed tasks */
    mstimer_callback(&BACnet_Callback, bacnet_task_timed, 5);
    for (;;) {
        bacnet_task();
        led_task();
    }
}
