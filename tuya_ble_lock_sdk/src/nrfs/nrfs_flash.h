/**
****************************************************************************
* @file      nrfs_flash.h
* @brief     nrfs_flash
* @author    suding
* @version   V1.0.0
* @date      2019-10-12
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2019 Tuya </center></h2>
*/


#ifndef __NRFS_FLASH_H__
#define __NRFS_FLASH_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "nrfs_common.h"

/*********************************************************************
 * CONSTANTS
 */
#define NRFS_FLASH_START_ADDR        0x46000
#define NRFS_FLASH_END_ADDR          0x78000

#define NRFS_FLASH_OTA_START_ADDR    NRFS_FLASH_START_ADDR
#define NRFS_FLASH_OTA_END_ADDR      0x66000

#define NRFS_FLASH_BLE_MAC_ADDR      0x77000

/*********************************************************************
 * STRUCT
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
void  nrfs_flash_init(void);
ret_code_t nrfs_flash_read(uint32_t addr, uint8_t *buf, uint32_t size);
ret_code_t nrfs_flash_write(uint32_t addr, uint8_t *buf, uint32_t size, bool block);
ret_code_t nrfs_flash_erase(uint32_t addr, uint32_t num, bool block);


#ifdef __cplusplus
}
#endif

#endif //__NRFS_FLASH_H__
