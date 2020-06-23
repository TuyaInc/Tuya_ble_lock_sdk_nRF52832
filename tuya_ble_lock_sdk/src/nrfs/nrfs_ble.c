#include "nrfs_ble.h"
#include "lock_timer.h"




/*********************************************************************
 * LOCAL CONSTANTS
 */
#define NRFS_BLE_OBSERVER_PRIO               3

#define NRFS_DEVICE_NAME                     "Lock_Common"

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
NRF_BLE_GATT_DEF(m_gatt);

static volatile uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;
static volatile uint16_t m_ble_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;
static volatile nrfs_conn_param_t g_defConn_param = NRFS_DEFAULT_CONN_PARAM;
static volatile nrfs_conn_param_t g_conn_param    = NRFS_DEFAULT_CONN_PARAM;

/*********************************************************************
 * LOCAL FUNCTION
 */
static void nrfs_ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);
static uint32_t nrfs_set_defConn_param(void);
static void gatt_evt_handler(nrf_ble_gatt_t* p_gatt, nrf_ble_gatt_evt_t const* p_evt);

/*********************************************************************
 * VARIABLES
 */




/*********************************************************
FN: Function for handling BLE events.
PM: p_ble_evt   Bluetooth stack event.
    p_context   Unused.
*/
static void nrfs_ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code = NRF_SUCCESS;

    ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED: {
            if(p_gap_evt->params.connected.role == BLE_GAP_ROLE_PERIPH)
            {
                NRFS_PRINTF("nrfs Connected");
                nrfs_adv_state_update(false);
                m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
                tuya_ble_connected_handler();
                app_common_evt_send_only_evt(APP_EVT_BLE_GAP_EVT_CONNECTED);
            }
        } break;

        case BLE_GAP_EVT_DISCONNECTED: {
            NRFS_PRINTF("nrfs Disconnected: 0x%02x", p_gap_evt->params.disconnected.reason);
            {
                g_sync_new.flag = 0;
            }
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            tuya_ble_disconnected_handler();
            app_common_evt_send_only_evt(APP_EVT_BLE_GAP_EVT_DISCONNECTED);
            
            nrfs_adv_start();
        } break;

        case BLE_GAP_EVT_TIMEOUT: {
            NRFS_PRINTF("Error: BLE_GAP_EVT_TIMEOUT.");
        } break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            NRFS_PRINTF("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST: {
            NRFS_PRINTF("sec param request.");
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING: {
            NRFS_PRINTF("sttr missing request.");
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(p_ble_evt->evt.gap_evt.conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTC_EVT_TIMEOUT: {
            // Disconnect on GATT Client timeout event.
            NRFS_PRINTF("Error: GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_TIMEOUT: {
            // Disconnect on GATT Server timeout event.
            NRFS_PRINTF("Error: GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE: {
            NRFS_PRINTF("BLE_GAP_EVT_CONN_PARAM_UPDATE: min-%dms, max-%dms, latency-%d, timeout-%dms", \
                (uint16_t)(p_gap_evt->params.conn_param_update.conn_params.min_conn_interval*1.25), \
                (uint16_t)(p_gap_evt->params.conn_param_update.conn_params.max_conn_interval*1.25), \
                (uint16_t)(p_gap_evt->params.conn_param_update.conn_params.slave_latency), \
                (uint16_t)(p_gap_evt->params.conn_param_update.conn_params.conn_sup_timeout*10) );
        } break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: {
            NRFS_PRINTF("BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST");
        } break;

        default: {
        } break;
    }
}

/*********************************************************
FN: 设置默认连接参数
*/
static uint32_t nrfs_set_defConn_param(void)
{
    ret_code_t err_code;

    //期望的连接参数
    ble_gap_conn_params_t gap_conn_params;
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));
    gap_conn_params.min_conn_interval = MSEC_TO_UNITS(g_defConn_param.cMin, UNIT_1_25_MS);
    gap_conn_params.max_conn_interval = MSEC_TO_UNITS(g_defConn_param.cMax, UNIT_1_25_MS);
    gap_conn_params.slave_latency     = g_defConn_param.latency;
    gap_conn_params.conn_sup_timeout  = MSEC_TO_UNITS(g_defConn_param.timeout, UNIT_10_MS);
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);

    return err_code;
}

/*********************************************************
FN: 设置当前连接参数
*/
ret_code_t nrfs_set_conn_param(void)
{
    ret_code_t err_code;

    //当前的连接参数
    ble_gap_conn_params_t gap_conn_params;
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));
    gap_conn_params.min_conn_interval = MSEC_TO_UNITS(g_conn_param.cMin, UNIT_1_25_MS);
    gap_conn_params.max_conn_interval = MSEC_TO_UNITS(g_conn_param.cMax, UNIT_1_25_MS);
    gap_conn_params.slave_latency     = g_conn_param.latency;
    gap_conn_params.conn_sup_timeout  = MSEC_TO_UNITS(g_conn_param.timeout, UNIT_10_MS);
    err_code = sd_ble_gap_conn_param_update(m_conn_handle, &gap_conn_params);
    APP_ERROR_CHECK(err_code);

    return err_code;
}

/*********************************************************
FN: 连接参数更新
*/
ret_code_t nrfs_conn_param_update(uint16_t cMin, uint16_t cMax, uint16_t latency, uint16_t timeout)
{
    ret_code_t err_code;

    g_conn_param.cMin = cMin;
    g_conn_param.cMax = cMax;
    g_conn_param.latency = latency;
    g_conn_param.timeout = timeout;
    err_code = nrfs_set_conn_param();
    APP_ERROR_CHECK(err_code);

    return err_code;
}

/*********************************************************
FN: init SoftDevice、BLE Stack， register BLE event callback
*/
void nrfs_ble_stack_init(void)
{
    ret_code_t err_code;

    //使能SoftDevice
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    //设置默认的Stack配置，获取APP程序RAM的起始地址
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(NRFS_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // 使能 BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // 注册BLE Event通知函数
    NRF_SDH_BLE_OBSERVER(m_ble_observer, NRFS_BLE_OBSERVER_PRIO, nrfs_ble_evt_handler, NULL);
}

/*********************************************************
FN: 初始化GAP（设备名称、外观、期望的连接参数）
*/
void nrfs_gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_sec_mode_t sec_mode;

    //安全模式
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    //设备名称
    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)NRFS_DEVICE_NAME, strlen(NRFS_DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    //设备外观
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    APP_ERROR_CHECK(err_code);

    //期望的连接参数
    nrfs_set_defConn_param();
}

/*********************************************************
FN: GATT module初始化（MTU和DLE）
*/
void nrfs_gatt_init(void)
{
    ret_code_t err_code;
    err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

//    //设置MTU
//    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, 128);
//    APP_ERROR_CHECK(err_code);
}

/*********************************************************
FN: Function for handling events from the GATT library.
*/
static void gatt_evt_handler(nrf_ble_gatt_t* p_gatt, nrf_ble_gatt_evt_t const* p_evt)
{
    if((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        m_ble_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        NRFS_PRINTF("Data len is set to 0x%X(%d)", m_ble_max_data_len, m_ble_max_data_len);
    }
    NRFS_PRINTF("ATT MTU exchange completed. central 0x%x peripheral 0x%x", p_gatt->att_mtu_desired_central, p_gatt->att_mtu_desired_periph);
}




/*********************************************************
FN: 断开连接
*/
ret_code_t nrfs_disconnect(uint16_t conn_handle, uint8_t hci_status_code)
{
    return sd_ble_gap_disconnect(m_conn_handle, hci_status_code);
}

/*********************************************************
FN: 
*/
void nrfs_init_bt_mac_addr(void)
{
    uint8_t auth = 0;
    //set bt addr
    uint8_t tmp_mac_str[APP_PORT_BLE_ADDR_STR_LEN] = APP_PORT_DEFAULT_MAC_ADDR_STR;
    uint8_t mac[APP_PORT_BLE_ADDR_LEN];
    uint8_t tmp_mac_in_flash[APP_PORT_BLE_ADDR_STR_LEN];
    uint8_t tmp_mac_invalid[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    nrfs_flash_read(NRFS_FLASH_BLE_MAC_ADDR, tmp_mac_in_flash, APP_PORT_BLE_ADDR_STR_LEN);
    if(memcmp(tmp_mac_in_flash, tmp_mac_invalid, APP_PORT_BLE_ADDR_STR_LEN) != 0) {
        memcpy(tmp_mac_str, tmp_mac_in_flash, APP_PORT_BLE_ADDR_STR_LEN);
        auth = 1;
    }
        
    if(app_port_string_op_hexstr2hex(tmp_mac_str, APP_PORT_BLE_ADDR_STR_LEN, mac) == 1)
    {
        if(!auth) {
            app_port_reverse_byte(mac, APP_PORT_BLE_ADDR_LEN);
        }
        APP_DEBUG_HEXDUMP("Mac", mac, APP_PORT_BLE_ADDR_LEN);
        app_port_set_bt_mac_addr(mac);
    }
}

/*********************************************************
FN: 
*/
ret_code_t nrfs_set_bt_mac_addr(uint8_t *addr)
{
    ret_code_t err_code;
    bool adv_flag = false;

    if(g_isAdvtising)
    {
        adv_flag = true;
        //停止广播
        err_code = nrfs_adv_stop();
        APP_ERROR_CHECK(err_code);
        if(err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }

    ble_gap_addr_t p_addr;
    memset(&p_addr, 0, sizeof(p_addr));
    p_addr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;
    memcpy(p_addr.addr, addr, BLE_GAP_ADDR_LEN);
    
    err_code = sd_ble_gap_addr_set(&p_addr);
    APP_ERROR_CHECK(err_code);

    if(adv_flag)
    {
        //启动广播
        err_code = nrfs_adv_start();
        APP_ERROR_CHECK(err_code);
    }
    
    return err_code;
}

/*********************************************************
FN: 
*/
ret_code_t nrfs_get_bt_mac_addr(uint8_t *addr, uint8_t len)
{
    ret_code_t err_code;
    ble_gap_addr_t mac;
    err_code = sd_ble_gap_addr_get(&mac);
    APP_ERROR_CHECK(err_code);
    
    memcpy(addr, mac.addr, BLE_GAP_ADDR_LEN);
    return err_code;
}

/*********************************************************
FN: 
*/
uint16_t get_m_conn_handle(void)
{
    return m_conn_handle;
}

/*********************************************************
FN: 
*/
uint16_t get_m_ble_max_data_len(void)
{
    return m_ble_max_data_len;
}



















