#include "nrfs_timer.h"




//在timer_id不变的情况下多次调用create接口存在隐患，需要测试验证，详见：https://www.cnblogs.com/iini/p/9347460.html 末尾常见使用问题

/*********************************************************************
 * LOCAL CONSTANTS
 */
#define NRFS_TIMER_MAX_NUM    20

/*********************************************************************
 * LOCAL STRUCT
 */
typedef struct
{
    uint8_t is_occupy;
    app_timer_t data;
    uint32_t ms;
} nrfs_timer_item_t;

/*********************************************************************
 * LOCAL VARIABLES
 */
static nrfs_timer_item_t m_timer_pool[NRFS_TIMER_MAX_NUM] = {0};

/*********************************************************************
 * LOCAL FUNCTION
 */

/*********************************************************************
 * VARIABLES
 */




/*********************************************************
FN: 
*/
static app_timer_id_t acquire_timer(uint32_t ms)
{
    for(uint8_t i=0; i<NRFS_TIMER_MAX_NUM; i++)
    {
        if(m_timer_pool[i].is_occupy == 0)
        {
            m_timer_pool[i].is_occupy = 1;
            m_timer_pool[i].ms = ms;
            return (void *)&m_timer_pool[i].data;
        }
    }
    return NULL;
}

/*********************************************************
FN: 
*/
static int32_t release_timer(void* timer_id)
{
    for(uint8_t i=0; i<NRFS_TIMER_MAX_NUM; i++)
    {
        if(timer_id == &m_timer_pool[i].data)
        {
            m_timer_pool[i].is_occupy = 0;
            return i;
        }
    }
    return -1;
}

/*********************************************************
FN: 
*/
static int32_t find_timer_ms(void* timer_id, uint32_t *ms)
{
    for(uint8_t i=0; i<NRFS_TIMER_MAX_NUM; i++)
    {
        if(timer_id == &m_timer_pool[i].data)
        {
            *ms = m_timer_pool[i].ms;
            return i;
        }
    }
    return -1;
}

/*********************************************************
FN: 
*/
ret_code_t nrfs_timer_create(void** p_timer_id, uint32_t timeout_value_ms, app_timer_mode_t mode, app_timer_timeout_handler_t timeout_handler)
{
    ret_code_t err_code;
    
    //首次初始化
    static uint8_t is_init = 0;
    if(is_init == 0)
    {
        err_code = app_timer_init();
        APP_ERROR_CHECK(err_code);
        if(err_code == NRF_SUCCESS)
        {
            is_init = 1;
        }
    }

    app_timer_id_t id = acquire_timer(timeout_value_ms);
    if(id == NULL)
    {
        NRFS_PRINTF("Error: App timer is used up");
        return NRF_ERROR_NO_MEM;
    }

    err_code = app_timer_create(&id, mode, timeout_handler);
    APP_ERROR_CHECK(err_code);
    
    *p_timer_id = id;
    
    return err_code;
}

/*********************************************************
FN: 
*/
ret_code_t nrfs_timer_delete(app_timer_id_t timer_id)
{
    ret_code_t err_code;
    
    int32_t id = release_timer(timer_id);
    if(id == -1)
    {
        NRFS_PRINTF("Error: Timer id is not valid");
        return NRF_ERROR_INVALID_PARAM;
    }

    err_code = app_timer_stop(timer_id);
    APP_ERROR_CHECK(err_code);
    return err_code;

}

/*********************************************************
FN: 
*/
ret_code_t nrfs_timer_start(app_timer_id_t timer_id)
{
    ret_code_t err_code;
    uint32_t ms;
    
    if(find_timer_ms(timer_id, &ms) >= 0)
    {
        err_code = app_timer_start(timer_id, APP_TIMER_TICKS(ms), NULL);
        APP_ERROR_CHECK(err_code);
        return err_code;
    }
    else
    {
        NRFS_PRINTF("Error: Timer id is not found");
        return NRF_ERROR_NOT_FOUND;
    }
}

/*********************************************************
FN: 
*/
ret_code_t nrfs_timer_stop(app_timer_id_t timer_id)
{
    ret_code_t err_code;
    err_code = app_timer_stop(timer_id);
    APP_ERROR_CHECK(err_code);
    return err_code; 
}




/*********************************************************  RTC 2  *********************************************************/

/* Declaring an instance of nrf_drv_rtc for RTC2. */
const nrf_drv_rtc_t rtc2 = NRF_DRV_RTC_INSTANCE(2);
static uint32_t s_local_timestamp = 0;
static uint32_t s_local_timestamp_when_update = 0;
static uint32_t s_app_timestamp_when_update = 0;

/*********************************************************
FN: 
*/
static void rtc2_handler(nrf_drv_rtc_int_type_t int_type)
{
    static uint16_t count = 0;
    if(int_type == NRF_DRV_RTC_INT_TICK)
    {
        count++;
        if(count == 8)
        {
            count=0;
            s_local_timestamp++;
        }
    }
}

/*********************************************************
FN: Function initialization and configuration of RTC driver instance.
*/
static void rtc2_config(void)
{
    uint32_t err_code;

    //Initialize RTC instance
    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
    config.prescaler = 4095;
    err_code = nrf_drv_rtc_init(&rtc2, &config, rtc2_handler);
    APP_ERROR_CHECK(err_code);

    //Enable tick event & interrupt
    nrf_drv_rtc_tick_enable(&rtc2, true);

    //Power on RTC instance
    nrf_drv_rtc_enable(&rtc2);
}

/*********************************************************
FN: 
*/
static void rtc2_start(void)
{
    rtc2_config();
}

/*********************************************************
FN: 启动本地时间戳
*/
void nrfs_local_timer_start(void)
{
    rtc2_start();
}

/*********************************************************
FN: 更新时间戳
*/
void nrfs_update_timestamp(uint32_t app_timestamp)
{
    s_local_timestamp_when_update = s_local_timestamp;
    s_app_timestamp_when_update = app_timestamp;
}

/*********************************************************
FN: 获取更新时的app时间戳
*/
uint32_t nrfs_get_app_timestamp_when_update(void)
{
    return s_app_timestamp_when_update;
}

/*********************************************************
FN: 获取本地时间戳
*/
uint32_t nrfs_get_local_timestamp(void)
{
    return s_local_timestamp;
}

/*********************************************************
FN: 获取当前时间戳（如果没有更新过，即为本地时间戳）
*/
uint32_t nrfs_get_timestamp(void)
{
    return (s_app_timestamp_when_update + (s_local_timestamp - s_local_timestamp_when_update));
}

/*********************************************************
FN: 获取过去的时间戳（必须在更新时间戳之后使用，否则返回 old_local_timestamp）
*/
uint32_t nrfs_get_old_timestamp(uint32_t old_local_timestamp)
{
    return (nrfs_get_timestamp() - (s_local_timestamp - old_local_timestamp));
}




/*********************************************************  delay  *********************************************************/

/*********************************************************
FN: 
*/
void nrfs_delay_ms(uint32_t ms)
{
    nrf_delay_ms(ms);
}

/*********************************************************
FN: 
*/
void nrfs_delay_us(uint32_t us)
{
    nrf_delay_us(us);
}











