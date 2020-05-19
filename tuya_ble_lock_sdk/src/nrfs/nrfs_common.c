#include "nrfs_common.h"




/*********************************************************************
 * LOCAL CONSTANTS
 */

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTION
 */

/*********************************************************************
 * VARIABLES
 */




/*********************************************************
FN: 
*/
void nrfs_log_init(void)
{
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT,  ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME|ELOG_FMT_FUNC|ELOG_FMT_LINE);
    elog_set_fmt(ELOG_LVL_ERROR,   ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME|ELOG_FMT_FUNC|ELOG_FMT_LINE);
    elog_set_fmt(ELOG_LVL_WARN,    ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME|ELOG_FMT_FUNC|ELOG_FMT_LINE);
    elog_set_fmt(ELOG_LVL_INFO,    ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG,   ELOG_FMT_LVL);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_LVL);
    /* start EasyLogger */
    elog_start();
}

/*********************************************************
FN: 
*/
void nrfs_log_hexdump(const char *name, uint8_t *buf, uint16_t size)
{
    elog_hexdump(name, 8, buf, size);
}

/*********************************************************
FN: 
*/
void nrfs_log_hexdump_empty(const char *name, uint8_t width, uint8_t *buf, uint16_t size)
{
    //empty
}

/*********************************************************
FN: 
*/
void nrfs_system_reset(void)
{
    NRF_LOG_FINAL_FLUSH();
    NVIC_SystemReset();
}

/*********************************************************
FN: 
*/
void nrfs_enter_critical(void)
{
   CRITICAL_REGION_ENTER();
}

/*********************************************************
FN: 
*/
void nrfs_exit_critical(void)
{
   CRITICAL_REGION_EXIT();
}




















