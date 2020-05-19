#include "nrfs_uart.h"




/*********************************************************************
 * LOCAL CONSTANTS
 */
#define UART_TX_BUF_SIZE                512
#define UART_RX_BUF_SIZE                512

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTION
 */
static void uart_event_handle(app_uart_evt_t * p_event);

/*********************************************************************
 * VARIABLES
 */




/*********************************************************
FN: Function for handling app_uart events.
*/
static void uart_event_handle(app_uart_evt_t * p_event)
{
    uint8_t rx_char;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY: {
            UNUSED_VARIABLE(app_uart_get(&rx_char));
            tuya_ble_uart_receive_data(&rx_char, 1);
        } break;

        case APP_UART_TX_EMPTY: {
            NRFS_PRINTF("uart tx finish");
        } break;

        case APP_UART_COMMUNICATION_ERROR: {
            APP_ERROR_HANDLER(p_event->data.error_communication);
        } break;

        case APP_UART_FIFO_ERROR: {
            APP_ERROR_HANDLER(p_event->data.error_code);
        } break;

        default: {
        } break;
    }
}

/*********************************************************
FN: Function for initializing the UART module.
*/
void nrfs_uart_init(void)
{
    ret_code_t err_code;
    app_uart_comm_params_t const comm_params =
    {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
        .baud_rate    = NRF_UART_BAUDRATE_9600
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}

/*********************************************************
FN: Function for initializing the UART module.
*/
ret_code_t nrfs_uart_send(const uint8_t* buf, uint32_t size)
{
    NRFS_HEXDUMP("TX", (void*)buf, size);
    ret_code_t err_code;
    for(uint32_t idx=0; idx<size; idx++)
    {
        err_code = app_uart_put(buf[idx]);
        APP_ERROR_CHECK(err_code);
        if(err_code != NRF_SUCCESS)
        {
            break;
        }
    }
    return err_code;
}






















