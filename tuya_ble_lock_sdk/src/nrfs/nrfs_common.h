/**
****************************************************************************
* @file      nrfs_common.h
* @brief     nrfs_common
* @author    suding
* @version   V1.0.0
* @date      2019-10-12
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2019 Tuya </center></h2>
*/


#ifndef __NRFS_COMMON_H__
#define __NRFS_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "nordic_common.h"
#include "nrf.h"
#include "bsp_btn_ble.h"
#include "nrfx_rtc.h"
#include "nrf_sdh.h"
#include "nrf_sdm.h"
#include "nrf_sdh_soc.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "nrf_sdh_ble.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_delay.h"
#include "nrf_uart.h"
#include "nrf_ble_qwr.h"
#include "nrf_ble_scan.h"
#include "nrf_ble_gatt.h"
#include "nrf_dfu_settings.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
//nrf_ble
#include "ble.h"
#include "ble_hci.h"
#include "ble_gap.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "ble_nus.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
//nrf_lib
#include "fds.h"
#include "SEGGER_RTT.h"
//nrf_app
#include "app_timer.h"
#include "app_uart.h"
#include "app_error.h"

//nrfs
#include "nrfs_scan_adv.h"
#include "nrfs_ble.h"
#include "nrfs_svc.h"
#include "nrfs_uart.h"
#include "nrfs_timer.h"
#include "nrfs_flash.h"
#include "nrfs_gpio.h"
#include "nrfs_test.h"
//cpt
#include "cpt_math.h"
#include "cpt_string_op.h"
#include "easyflash.h"
#include "elog.h"

//tuya_ble_sdk
#include "tuya_ble_api.h"

/*********************************************************************
 * CONSTANTS
 */
#define NRFS_DEBUG_EN 1

#if (NRFS_DEBUG_EN)

    #define NRFS_PRINTF       log_d
    #define NRFS_HEXDUMP      nrfs_log_hexdump
    
#else

    #define NRFS_PRINTF(...)
    #define NRFS_HEXDUMP      nrfs_log_hexdump_empty
    
#endif

/*********************************************************************
 * STRUCT
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
void nrfs_log_init(void);
void nrfs_log_hexdump(const char *name, uint8_t *buf, uint16_t size);
void nrfs_log_hexdump_empty(const char *name, uint8_t *buf, uint16_t size);

void nrfs_system_reset(void);
void nrfs_enter_critical(void);
void nrfs_exit_critical(void);

#ifdef __cplusplus
}
#endif

#endif //__NRFS_COMMON_H__
