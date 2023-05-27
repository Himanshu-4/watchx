#ifndef _BLE_STACK_CONF_
#define _BLE_STACK_CONF_



/**
 * This macro exists to ensure code includes this header when needed.  If code
 * checks the existence of a setting directly via ifdef without including this
 * header, the setting macro will silently evaluate to 0.  In contrast, an
 * attempt to use these macros without including this header will result in a
 * compiler error.
 */
#define MYNEWT_VAL(_name)                       MYNEWT_VAL_ ## _name
#define MYNEWT_VAL_CHOICE(_name, _val)          MYNEWT_VAL_ ## _name ## __ ## _val



// /*** @apache-mynewt-core/crypto/tinycrypt */
// #ifndef MYNEWT_VAL_TINYCRYPT_SYSINIT_STAGE
// #define MYNEWT_VAL_TINYCRYPT_SYSINIT_STAGE (200)
// #endif

// #ifndef MYNEWT_VAL_TINYCRYPT_UECC_RNG_TRNG_DEV_NAME
// #define MYNEWT_VAL_TINYCRYPT_UECC_RNG_TRNG_DEV_NAME ("trng")
// #endif

// #define MYNEWT_VAL_TINYCRYPT_UECC_RNG_USE_TRNG (0)

// /*** @apache-mynewt-core/hw/hal */
// #define MYNEWT_VAL_HAL_ENABLE_SOFTWARE_BREAKPOINTS (1)


// #define MYNEWT_VAL_HAL_FLASH_VERIFY_BUF_SZ (16)


// #define MYNEWT_VAL_HAL_FLASH_VERIFY_ERASES (0)


// #define MYNEWT_VAL_HAL_FLASH_VERIFY_WRITES (0)

// #define MYNEWT_VAL_HAL_SYSTEM_RESET_CB (0)


// /*** @apache-mynewt-core/kernel/os */

// #define MYNEWT_VAL_FLOAT_USER (0)

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// driver related //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
        /*Enable SystemView tracing module for radio driver.
        */
#define MYNEWT_VAL_BLE_PHY_SYSVIEW (0)

/*
            This defines additional margin for T_IFS tolerance while in
            RX on coded phy to allow maintaining connections with some
            controllers that exceed proper T_IFS (150 usecs) by more
            than allowed 2 usecs.
            This value shall be only used for debugging purposes. It is
            strongly recommended to keep this settings at default value
            to ensure compliance with specification.
*/
#define MYNEWT_VAL_BLE_PHY_CODED_RX_IFS_EXTRA_MARGIN (0)
  
/*
            When set to proper GPIO pin number, this pin will be set
            to high state when radio is enabled using PPI channels
            20 or 21 and back to low state on radio EVENTS_READY.
            This can be used to measure radio ram-up time.
  */      
#define MYNEWT_VAL_BLE_PHY_DBG_TIME_TXRXEN_READY_PIN (-1)

 /*
            When set to proper GPIO pin number, this pin will be set
            to high state on radio EVENTS_ADDRESS and back to low state
            on radio EVENTS_END.
            This can be used to measure radio pipeline delays.
        */
#define MYNEWT_VAL_BLE_PHY_DBG_TIME_ADDRESS_END_PIN (-1)

/*
            When set to proper GPIO pin number, this pin will be set
            to high state on radio EVENTS_RXREADY and back to low
            state when wfr timer expires.
            This can be used to check if wfr is calculated properly.
*/
#define MYNEWT_VAL_BLE_PHY_DBG_TIME_WFR_PIN (-1)

/*
            Enable workaround for anomaly 164 found in nRF52840.
            "[164] RADIO: Low selectivity in long range mode"
            This shall be only enabled for:
            - nRF52840 Engineering A
  */
#define MYNEWT_VAL_BLE_PHY_NRF52840_ERRATA_164 (0)

/*
            Enable workaround for anomaly 191 found in nRF52840.
            "[191] RADIO: High packet error rate in BLE Long Range mode"
            This shall be only enabled for:
            - nRF52840 Engineering B
            - nRF52840 Engineering C
            - nRF52840 Rev 1 (final silicon)
*/
  #define MYNEWT_VAL_BLE_PHY_NRF52840_ERRATA_191 (1)

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/////////////////// os related settings /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


#define MYNEWT_VAL_MSYS_1_BLOCK_COUNT (30)


#define MYNEWT_VAL_MSYS_1_BLOCK_SIZE (292)


#define MYNEWT_VAL_MSYS_1_SANITY_MIN_COUNT (0)


#define MYNEWT_VAL_MSYS_2_BLOCK_COUNT (0)


#define MYNEWT_VAL_MSYS_2_BLOCK_SIZE (0)


#define MYNEWT_VAL_MSYS_2_SANITY_MIN_COUNT (0)


#define MYNEWT_VAL_MSYS_SANITY_TIMEOUT (60000)


#define MYNEWT_VAL_OS_ASSERT_CB (0)


#define MYNEWT_VAL_OS_CLI (0)


#define MYNEWT_VAL_OS_COREDUMP (0)


#define MYNEWT_VAL_OS_CPUTIME_FREQ (32768)


#define MYNEWT_VAL_OS_CPUTIME_TIMER_NUM (5)


// #define MYNEWT_VAL_OS_CRASH_FILE_LINE (0)


// #define MYNEWT_VAL_OS_CRASH_LOG (0)


// #define MYNEWT_VAL_OS_CRASH_RESTORE_REGS (0)


// #define MYNEWT_VAL_OS_CRASH_STACKTRACE (0)


// #define MYNEWT_VAL_OS_CTX_SW_STACK_CHECK (0)


// #define MYNEWT_VAL_OS_CTX_SW_STACK_GUARD (4)


// #define MYNEWT_VAL_OS_DEBUG_MODE (0)


// #define MYNEWT_VAL_OS_EVENTQ_DEBUG (0)


// #define MYNEWT_VAL_OS_EVENTQ_MONITOR (0)


// #define MYNEWT_VAL_OS_IDLE_TICKLESS_MS_MAX (600000)


// #define MYNEWT_VAL_OS_IDLE_TICKLESS_MS_MIN (100)


// #define MYNEWT_VAL_OS_MAIN_STACK_SIZE (1024)


// #define MYNEWT_VAL_OS_MAIN_TASK_PRIO (127)


// #define MYNEWT_VAL_OS_MAIN_TASK_SANITY_ITVL_MS (0)


// #define MYNEWT_VAL_OS_MEMPOOL_CHECK (0)


// #define MYNEWT_VAL_OS_MEMPOOL_GUARD (0)


// #define MYNEWT_VAL_OS_MEMPOOL_POISON (0)


// #define MYNEWT_VAL_OS_SCHEDULING (1)


// #define MYNEWT_VAL_OS_SYSINIT_STAGE (0)


// #define MYNEWT_VAL_OS_SYSVIEW (0)


// #define MYNEWT_VAL_OS_SYSVIEW_TRACE_CALLOUT (1)


// #define MYNEWT_VAL_OS_SYSVIEW_TRACE_EVENTQ (1)


// #define MYNEWT_VAL_OS_SYSVIEW_TRACE_MBUF (0)


// #define MYNEWT_VAL_OS_SYSVIEW_TRACE_MEMPOOL (0)


// #define MYNEWT_VAL_OS_SYSVIEW_TRACE_MUTEX (1)


// #define MYNEWT_VAL_OS_SYSVIEW_TRACE_SEM (1)


// #define MYNEWT_VAL_OS_TASK_RUN_TIME_CPUTIME (0)


// #define MYNEWT_VAL_OS_TIME_DEBUG (0)


// #define MYNEWT_VAL_OS_WATCHDOG_MONITOR (0)


// #define MYNEWT_VAL_SANITY_INTERVAL (15000)


// #define MYNEWT_VAL_WATCHDOG_INTERVAL (30000)

// /*** @apache-mynewt-core/sys/console/stub */

// // #define MYNEWT_VAL_CONSOLE_UART_BAUD (115200)


// // #define MYNEWT_VAL_CONSOLE_UART_DEV ("uart0")


// // #define MYNEWT_VAL_CONSOLE_UART_FLOW_CONTROL (UART_FLOW_CTL_NONE)

// /*** @apache-mynewt-core/sys/flash_map */

// // #define MYNEWT_VAL_FLASH_MAP_MAX_AREAS (10)


// // #define MYNEWT_VAL_FLASH_MAP_SYSINIT_STAGE (2)

// /*** @apache-mynewt-core/sys/log/common */

// #define MYNEWT_VAL_DFLT_LOG_LVL (1)


// #define MYNEWT_VAL_DFLT_LOG_MOD (0)


// #define MYNEWT_VAL_LOG_GLOBAL_IDX (1)

// /*** @apache-mynewt-core/sys/log/modlog */

// #define MYNEWT_VAL_MODLOG_CONSOLE_DFLT (1)


// #define MYNEWT_VAL_MODLOG_LOG_MACROS (0)


// #define MYNEWT_VAL_MODLOG_MAX_MAPPINGS (16)


// #define MYNEWT_VAL_MODLOG_MAX_PRINTF_LEN (128)


// #define MYNEWT_VAL_MODLOG_SYSINIT_STAGE (100)

// /*** @apache-mynewt-core/sys/log/stub */

// // #define MYNEWT_VAL_LOG_CONSOLE (1)


// // #define MYNEWT_VAL_LOG_FCB (0)


// // #define MYNEWT_VAL_LOG_FCB_SLOT1 (0)


// #define MYNEWT_VAL_LOG_LEVEL (6)

// /*** @apache-mynewt-core/sys/mfg */

// #define MYNEWT_VAL_MFG_LOG_LVL (15)


// #define MYNEWT_VAL_MFG_LOG_MODULE (128)


// #define MYNEWT_VAL_MFG_MAX_MMRS (2)


// #define MYNEWT_VAL_MFG_SYSINIT_STAGE (100)





// /*** @apache-mynewt-core/sys/sys */

// #define MYNEWT_VAL_DEBUG_PANIC_ENABLED (1)

// /*** @apache-mynewt-core/sys/sysdown */

// #define MYNEWT_VAL_SYSDOWN_CONSTRAIN_DOWN (1)


// #define MYNEWT_VAL_SYSDOWN_PANIC_FILE_LINE (0)


// #define MYNEWT_VAL_SYSDOWN_PANIC_MESSAGE (0)


// #define MYNEWT_VAL_SYSDOWN_TIMEOUT_MS (10000)

// /*** @apache-mynewt-core/sys/sysinit */

// #define MYNEWT_VAL_SYSINIT_CONSTRAIN_INIT (1)


// #define MYNEWT_VAL_SYSINIT_PANIC_FILE_LINE (0)


// #define MYNEWT_VAL_SYSINIT_PANIC_MESSAGE (0)

// /*** @apache-mynewt-core/util/rwlock */

// #define MYNEWT_VAL_RWLOCK_DEBUG (0)



////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////// normal BLE settings 
////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#define MYNEWT_VAL_BLE_ROLE_BROADCASTER (1) 


#define MYNEWT_VAL_BLE_ROLE_CENTRAL (0)


#define MYNEWT_VAL_BLE_ROLE_OBSERVER (0)


#define MYNEWT_VAL_BLE_ROLE_PERIPHERAL (1)


#define MYNEWT_VAL_BLE_VERSION (50)


#define MYNEWT_VAL_BLE_WHITELIST (1)

// /*** @apache-mynewt-nimble/nimble/controller */

#define MYNEWT_VAL_BLE_DEVICE (1)


// /* Overridden by @apache-mynewt-nimble/nimble/controller (defined by @apache-mynewt-nimble/nimble/controller) */
// enable the hardware whitelist 
#define MYNEWT_VAL_BLE_HW_WHITELIST_ENABLE (1)


// /*** @apache-mynewt-nimble/nimble */

#define MYNEWT_VAL_BLE_EXT_ADV (0)


#define MYNEWT_VAL_BLE_EXT_ADV_MAX_SIZE (31)


#define MYNEWT_VAL_BLE_MAX_CONNECTIONS (4)


#define MYNEWT_VAL_BLE_MAX_PERIODIC_SYNCS (1)


/*This is the number of multi-advertising instances. This is NOT the
            total number of advertising instances. The total number of
            advertising instances is this number plus 1 (assuming the device
            supports advertising).*/
#define MYNEWT_VAL_BLE_MULTI_ADV_INSTANCES (0)


#define MYNEWT_VAL_BLE_PERIODIC_ADV (0)


#define MYNEWT_VAL_BLE_PERIODIC_ADV_SYNC_TRANSFER (0)


#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LL_PERIODIC_ADV              MYNEWT_VAL_BLE_PERIODIC_ADV

#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LL_PERIODIC_ADV_SYNC_CNT          MYNEWT_VAL_BLE_MAX_PERIODIC_SYNCS


#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LL_PERIODIC_ADV_SYNC_LIST_CNT   MYNEWT_VAL_BLE_MAX_PERIODIC_SYNCS



#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LL_PERIODIC_ADV_SYNC_TRANSFER  MYNEWT_VAL_BLE_PERIODIC_ADV_SYNC_TRANSFER


/*Enable controller-to-host flow control support. This allows host to
  limit number of ACL packets sent at once from controller to avoid
  congestion on HCI transport if feature is also supported by host.*/
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_CTRL_TO_HOST_FLOW_CONTROL  (0)


/*This option is used to enable/disable support for SCA update procedure
*/
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LL_SCA_UPDATE (0)





/*
 # Configuration for LL supported features.
    #
    # There are a total 8 features that the LL can support. These can be found
    # in v4.2, Vol 6 Part B Section 4.6.
    #
    # These feature definitions are used to inform a host or other controller
    # about the LL features supported by the controller.
    #
    # NOTE: 'the' controller always supports extended reject indicate and thus
    # is not listed here.
    */

/*
 This option enables/disables the connection parameter request
            procedure.  This is implemented in the controller but is disabled
            by default.*/
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_CONN_PARAM_REQ (1)

/* This option enables/disables the data length update procedure in
the controller. If enabled, the controller is allowed to change the
size of tx/rx pdu's used in a connection. This option has only
minor impact on code size and non on RAM.*/
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_DATA_LEN_EXT (1)


// #define MYNEWT_VAL_BLE_LL_CFG_FEAT_EXT_SCAN_FILT (0)


#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LE_2M_PHY (1)


#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LE_CODED_PHY (0)

// /* Overridden by @apache-mynewt-nimble/nimble/controller (defined by @apache-mynewt-nimble/nimble/controller) */
/*  This option is used to enable/disable support for LE Channel
Selection Algorithm #2.*/
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LE_CSA2 (1)

/*This option enables/disables encryption support in the controller.
This option saves both both code and RAM.*/
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LE_ENCRYPTION (1)

/*  This option allows a controller to send/receive LE pings.
 Currently, this feature is not implemented by the controller so
turning it on or off has no effect.*/
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LE_PING (MYNEWT_VAL_BLE_LL_CFG_FEAT_LE_ENCRYPTION)


#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LL_EXT_ADV (MYNEWT_VAL_BLE_EXT_ADV)


#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LL_PRIVACY (1)

/* This option allows a slave to initiate the feature exchange
procedure.  This feature is implemented but currently has no impact
on code or ram size
            */
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_SLAVE_INIT_FEAT_XCHG (1)

/// used to set the initial max transmit PDU 
#define MYNEWT_VAL_BLE_LL_CONN_INIT_MAX_TX_BYTES (27)



//  # The number of slots that will be allocated to each connection
    /*This is the number of "slots" allocated to a connection when scheduling
connections. Each slot is 1.25 msecs long. Note that a connection event may
last longer than the number of slots allocated here and may also end earlier
(depending on when the next scheduled event occurs and how much data needs
to be transferred in the connection). However, you will be guaranteed that
a connection event will be given this much time, if needed. Consecutively
scheduled items will be at least this far apart
*/
#define MYNEWT_VAL_BLE_LL_CONN_INIT_SLOTS (4)

/*    This is the minimum number of "slots" for WindowOffset value used for
            CONNECT_IND when creating new connection as a master. Each slot is 1.25
            msecs long. Increasing this value will delay first connection event after
            connection is created. However, older TI CC254x controllers cannot change
            connection parameters later if WindowOffset was set to 0 in CONNECT_IND. To
            ensure interoperability with such devices set this value to 2 (or more).*/
#define MYNEWT_VAL_BLE_LL_CONN_INIT_MIN_WIN_OFFSET (0)

/*Forces the scheduler on a central to schedule connections in fixed
time intervals called periods. If set to 0, the scheduler is not forced
to do this. If set to 1, the scheduler will only schedule connections at
period boundaries. See comments in ble_ll_sched.h for more details.
        */
#define MYNEWT_VAL_BLE_LL_STRICT_CONN_SCHEDULING (0)

/*
The number of additional periods that will be allocated for strict
scheduling. The total # of periods allocated for strict scheduling
will be equal to the number of connections plus this number.*/
#define MYNEWT_VAL_BLE_LL_ADD_STRICT_SCHED_PERIODS (0)


#define MYNEWT_VAL_BLE_LL_DTM (0)

#define MYNEWT_VAL_BLE_LL_DIRECT_TEST_MODE (0)

#define MYNEWT_VAL_BLE_LL_DTM_EXTENSIONS (0)


// /* Overridden by @apache-mynewt-nimble/nimble/controller (defined by @apache-mynewt-nimble/nimble/controller) */

// #define MYNEWT_VAL_BLE_LL_EXT_ADV_AUX_PTR_CNT (5)


#define MYNEWT_VAL_BLE_LL_MASTER_SCA (4)


#define MYNEWT_VAL_BLE_LL_MAX_PKT_SIZE (251)


#define MYNEWT_VAL_BLE_LL_MFRG_ID (0xFFFF)


#define MYNEWT_VAL_BLE_LL_NUM_SCAN_DUP_ADVS (8)


#define MYNEWT_VAL_BLE_LL_NUM_SCAN_RSP_ADVS (8)


#define MYNEWT_VAL_BLE_LL_SCA (60)

#define MYNEWT_VAL_BLE_LL_OUR_SCA (60)


#define MYNEWT_VAL_BLE_LL_PRIO (0)


#define MYNEWT_VAL_BLE_LL_RESOLV_LIST_SIZE (4)


/*The number of random bytes that the link layer will try to
always have available for the host to use. Decreasing this
value may cause host delays if the host needs lots of random
material often.*/
#define MYNEWT_VAL_BLE_LL_RNG_BUFSIZE (32)
// 

/*    Time required for radio and/or related components to be fully
enabled before any request from LL is sent. This value is used
by rfmgmt to enable PHY in advance, before request from LL is
made. It depends on radio driver selected and may also depend
on hardware used:
- nrf51 - time required for XTAL to settle
- nrf52 - time required for XTAL to settle
Value is specified in microseconds. If set to 0, rfmgmt keeps
PHY enabled all the time.*/
#define MYNEWT_VAL_BLE_LL_RFMGMT_ENABLE_TIME (0)


#define MYNEWT_VAL_BLE_LL_SUPP_MAX_RX_BYTES (MYNEWT_VAL_BLE_LL_MAX_PKT_SIZE)


#define MYNEWT_VAL_BLE_LL_SUPP_MAX_TX_BYTES (MYNEWT_VAL_BLE_LL_MAX_PKT_SIZE)


// #define MYNEWT_VAL_BLE_LL_SYSVIEW (0)

// tx power level default 
#define MYNEWT_VAL_BLE_LL_TX_PWR_DBM (0)

// interval at which ll sends completed  packets to  host 
#define MYNEWT_VAL_BLE_LL_NUM_COMP_PKT_ITVL_MS     (2000)

/*The number of usecs per period.*/
#define MYNEWT_VAL_BLE_LL_USECS_PER_PERIOD (3250)

/*  This options enables controller to send a vendor-specific event on
an assertion in controller code. The event contains file name and
line number where assertion occured.*/
#define MYNEWT_VAL_BLE_LL_VND_EVENT_ON_ASSERT (0)

/* GPIO pin number to debug HCI commands flow. Pin is set to high state
            when HCI command is being processed.*/
#define MYNEWT_VAL_BLE_LL_DEBUG_GPIO_HCI_CMD (-1)

/*      GPIO pin number to debug HCI events flow. Pin is set to high state
            when HCI event is being sent.*/
#define MYNEWT_VAL_BLE_LL_DEBUG_GPIO_HCI_EV (-1)

/*  GPIO pin number to debug scheduler running (on timer). Pin is set
            to high state while scheduler is running.*/
#define MYNEWT_VAL_BLE_LL_DEBUG_GPIO_SCHED_RUN (-1)

/* GPIO pin number to debug scheduler item execution times. Pin is set
            to high state while item is executed.*/
#define MYNEWT_VAL_BLE_LL_DEBUG_GPIO_SCHED_ITEM_CB (-1)


#define MYNEWT_VAL_BLE_LL_WHITELIST_SIZE (8)

/*    This option is used to enable/disable support for LE Isochronous Channels
            as per Bluetooth v5.2 channels*/
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LL_ISO (0)


#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LL_ISO_TEST (0)

/*This option configure a max number of scheduled outstanding auxiliary
            packets for receive on secondary advertising channel.*/
#define MYNEWT_VAL_BLE_LL_EXT_ADV_AUX_PTR_CNT (0)












///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

// # Below settings allow to change scheduler timings. These should be left at
// # default values unless you know what you are doing!
        
    /*description: >
            Additional delay [us] between last ADV_EXT_IND and AUX_ADV_IND PDUs
            when scheduling extended advertising event. This extends T_MAFS.
        value: 0*/
#define MYNEWT_VAL_BLE_LL_SCHED_AUX_MAFS_DELAY (0)
        
/*            Additional delay [us] between consecutive AUX_CHAIN_IND PDUs
            when scheduling extended or periodic advertising event. This extends
            T_MAFS.*/
#define MYNEWT_VAL_BLE_LL_SCHED_AUX_CHAIN_MAFS_DELAY (0)
    
    /*This is expected PDU len for AUX_ADV_IND and subsequent
            AUX_CHAIN_IND. When scheduling scan scheduler will reserve time for
            receiving this amount of time. Setting this to high value improves
            reception of large PDUs but results in wasting scheduler space when
            receiving small PDUs only. On the other hand too low value can
            result in not being able to scan whole PDU due to being preempted
            by next scheduled item. By default size matching legacy ADV_IND PDU
            payload is used: ExtHeader (Flags, AdvA, ADI) + 31 bytes of data.
        range: 1..257
        value: 41*/
#define MYNEWT_VAL_BLE_LL_SCHED_SCAN_AUX_PDU_LEN (41)

        
/*This is expected PDU len for AUX_SYNC_IND and subsequent
            AUX_CHAIN_IND. When scheduling scan scheduler will reserve time for
            receiving this amount of time. Setting this to high value improves
            reception of large PDUs but results in wasting scheduler space when
            receiving small PDUs only. On the other hand too low value can
            result in not being able to scan whole PDU due to being preempted
            by next scheduled item. By default size matching PDU with legacy
            data size is used: ExtHeader + 31 bytes of data.
        range: 1..257
        value: 32*/
#define  MYNEWT_VAL_BLE_LL_SCHED_SCAN_SYNC_PDU_LEN (32)




#define MYNEWT_VAL_BLE_LP_CLOCK (1)


#define MYNEWT_VAL_BLE_NUM_COMP_PKT_RATE ((2 * OS_TICKS_PER_SEC))


#define MYNEWT_VAL_BLE_PUBLIC_DEV_ADDR ((uint8_t[6]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00})


#define MYNEWT_VAL_BLE_XTAL_SETTLE_TIME (0)




///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// ble nimble host /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


// /*** @apache-mynewt-nimble/nimble/host */

#define MYNEWT_VAL_BLE_ATT_PREFERRED_MTU (256)

/*Enables processing of incoming Find Information Request ATT
            commands. (0/1)*/
#define MYNEWT_VAL_BLE_ATT_SVR_FIND_INFO (1)

/*Enables processing of incoming Find By Type Value Request ATT
            commands. (0/1)*/
#define MYNEWT_VAL_BLE_ATT_SVR_FIND_TYPE (1)

/*Enables processing of incoming Handle Value Indication ATT
            commands.  (0/1)*/
#define MYNEWT_VAL_BLE_ATT_SVR_INDICATE (1)

/* A GATT server uses these when a peer performs a "write long
            characteristic values" or "write long characteristic descriptors"
            procedure.  One of these resources is consumed each time a peer
            sends a partial write.*/
#define MYNEWT_VAL_BLE_ATT_SVR_MAX_PREP_ENTRIES (64)

/* Enables processing of incoming Handle Value Notification ATT
            commands. (0/1)*/
#define MYNEWT_VAL_BLE_ATT_SVR_NOTIFY (1)

/* Enables processing of incoming Prepare Write Request and Execute
            Write Request ATT commands. (0/1)*/
#define MYNEWT_VAL_BLE_ATT_SVR_QUEUED_WRITE (1)

/* Expiry time for incoming ATT queued writes (ms).  If this much
            time passes since the previous prepared write was received, the
            connection is terminated.  A value of 0 means no timeout.*/
#define MYNEWT_VAL_BLE_ATT_SVR_QUEUED_WRITE_TMO (30000)

/*  Enables processing of incoming Read Request ATT commands. (0/1)*/
#define MYNEWT_VAL_BLE_ATT_SVR_READ (1)

/* Enables processing of incoming Read Blob Request ATT commands.*/
#define MYNEWT_VAL_BLE_ATT_SVR_READ_BLOB (1)

/*  Enables processing of incoming Read by Group Type Request ATT
            commands. */
#define MYNEWT_VAL_BLE_ATT_SVR_READ_GROUP_TYPE (1)

/* Enables processing of incoming Read Multiple Request ATT commands.*/
#define MYNEWT_VAL_BLE_ATT_SVR_READ_MULT (1)


#define MYNEWT_VAL_BLE_ATT_SVR_READ_TYPE (1)

/*Enables processing of incoming Signed Write Command ATT commands.*/
#define MYNEWT_VAL_BLE_ATT_SVR_SIGNED_WRITE (1)

/*    Enables processing of incoming Write Request ATT commands. (0/1)*/
#define MYNEWT_VAL_BLE_ATT_SVR_WRITE (1)

/* Enables processing of incoming Write Command ATT commands. (0/1)*/
#define MYNEWT_VAL_BLE_ATT_SVR_WRITE_NO_RSP (1)


/*** @apache-mynewt-nimble/nimble/host/services/gap */
///////////////////////////////////// gap layer ////////////////////////////
#define MYNEWT_VAL_BLE_SVC_GAP_APPEARANCE (0)


#define MYNEWT_VAL_BLE_SVC_GAP_APPEARANCE_WRITE_PERM (-1)


#define MYNEWT_VAL_BLE_SVC_GAP_CENTRAL_ADDRESS_RESOLUTION (-1)


#define MYNEWT_VAL_BLE_SVC_GAP_DEVICE_NAME ("nimble")


#define MYNEWT_VAL_BLE_SVC_GAP_DEVICE_NAME_MAX_LENGTH (31)


#define MYNEWT_VAL_BLE_SVC_GAP_DEVICE_NAME_WRITE_PERM (-1)


#define MYNEWT_VAL_BLE_SVC_GAP_PPCP_MAX_CONN_INTERVAL (0)


#define MYNEWT_VAL_BLE_SVC_GAP_PPCP_MIN_CONN_INTERVAL (0)


#define MYNEWT_VAL_BLE_SVC_GAP_PPCP_SLAVE_LATENCY (0)


#define MYNEWT_VAL_BLE_SVC_GAP_PPCP_SUPERVISION_TMO (0)


#define MYNEWT_VAL_BLE_SVC_GAP_SYSINIT_STAGE (301)

/* Controls the number of connection parameter updates that can be pending
            simultaneously. Devices with many concurrent connections may need
            to increase this value.*/
#define MYNEWT_VAL_BLE_GAP_MAX_PENDING_CONN_PARAM_UPDATE (1)

///////////////////////////////////////////////////////////////////////
///////////////////// gatt layer ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////


#define MYNEWT_VAL_BLE_GATT_DISC_ALL_CHRS (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_DISC_ALL_DSCS (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_DISC_ALL_SVCS (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_DISC_CHR_UUID (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_DISC_SVC_UUID (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_FIND_INC_SVCS (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_INDICATE (1)

/*  The maximum number of concurrent client GATT procedures. (0/1)*/
#define MYNEWT_VAL_BLE_GATT_MAX_PROCS (4)


#define MYNEWT_VAL_BLE_GATT_NOTIFY (1)


#define MYNEWT_VAL_BLE_GATT_READ (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_READ_LONG (MYNEWT_VAL_BLE_ROLE_CENTRAL)

/*The maximum number of attributes that can be read with a single
            GATT Read Multiple Characteristic Values procedure. (0/1)*/
#define MYNEWT_VAL_BLE_GATT_READ_MAX_ATTRS (8)

/* The maximum number of attributes that can be written with a single
            GATT Reliable Write procedure. (0/1)*/
#define MYNEWT_VAL_BLE_GATT_WRITE_MAX_ATTRS (4)

#define MYNEWT_VAL_BLE_GATT_READ_MULT (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_READ_UUID (MYNEWT_VAL_BLE_ROLE_CENTRAL)

/*   The rate to periodically resume GATT procedures that have stalled
            due to memory exhaustion. (0/1)  Units are milliseconds. (0/1)*/
#define MYNEWT_VAL_BLE_GATT_RESUME_RATE (1000)


#define MYNEWT_VAL_BLE_GATT_SIGNED_WRITE (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_WRITE (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_WRITE_LONG (MYNEWT_VAL_BLE_ROLE_CENTRAL)




#define MYNEWT_VAL_BLE_GATT_WRITE_NO_RSP (MYNEWT_VAL_BLE_ROLE_CENTRAL)


#define MYNEWT_VAL_BLE_GATT_WRITE_RELIABLE (MYNEWT_VAL_BLE_ROLE_CENTRAL)

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#define MYNEWT_VAL_BLE_HOST (1)


#define MYNEWT_VAL_BLE_HS_AUTO_START (1)

/////////// used to debug the host 
#define MYNEWT_VAL_BLE_HS_DEBUG (0)

/*   Whether to enable host-side flow control.  This should only be
            enabled in host-only setups (i.e., not combined-host-controller).*/
#define MYNEWT_VAL_BLE_HS_FLOW_CTRL (0)

/*The interval, in milliseconds, that the host should provide
            number-of-completed-packets updates to the controller.*/
#define MYNEWT_VAL_BLE_HS_FLOW_CTRL_ITVL (1000)

/* If the number of data buffers available to the controller falls to
            this number, immediately send a number-of-completed-packets event.
            The free buffer count is calculated as follows:
            (total-acl-bufs - bufs-freed-since-last-num-completed-event).*/
#define MYNEWT_VAL_BLE_HS_FLOW_CTRL_THRESH (2)

/* If enabled, the host will immediately transmit a
            host-number-of-completed-packets command whenever a connection
            terminates.  This behavior is not required by the standard, but is
            a necessary workaround when interfacing with some controllers.*/
#define MYNEWT_VAL_BLE_HS_FLOW_CTRL_TX_ON_DISCONNECT (0)

/* 'Minimum level for the BLE host log.'*/
#define MYNEWT_VAL_BLE_HS_LOG_LVL (1)

/*Numeric module ID to use for BLE host log messages.'*/
#define MYNEWT_VAL_BLE_HS_LOG_MOD (4)

/* Rather than wait for HCI acknowledgements from a controller, the
            host simulates incoming acks.  Only recommended for test code
            running in the simulator.*/
#define MYNEWT_VAL_BLE_HS_PHONY_HCI_ACKS (0)

/* Specifies whether the host can depend on the kernel being present.
            This should only be disabled for unit tests running in the
            simulator.*/
#define MYNEWT_VAL_BLE_HS_REQUIRE_OS (1)

/*  Stops the Bluetooth host when the system shuts down.  Stopping
            entails aborting all GAP procedures and terminating open
            connections.*/
#define MYNEWT_VAL_BLE_HS_STOP_ON_SHUTDOWN (1)

/*Timeout used in NimBLE's host stop procedure in ms.*/
#define MYNEWT_VAL_BLE_HS_STOP_ON_SHUTDOWN_TIMEOUT (2000)

/*Sysinit stage for the NimBLE host.*/
#define MYNEWT_VAL_BLE_HS_SYSINIT_STAGE (200)



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
///////////////////////////////////// the L2cap layer of tjhe host 


/*  Defines maximum number of LE Connection Oriented Channels channels.
            When set to (0), LE COC is not compiled in.*/
#define MYNEWT_VAL_BLE_L2CAP_COC_MAX_NUM (0)

/* Defines the MPS of L2CAP COC module. This is actually NimBLE's internal
            L2CAP MTU. The default MPS size is chosen in a way, that the MPS plus
            the required HCI and L2CAP headers fit into the smallest available
            MSYS blocks.*/
#define MYNEWT_VAL_BLE_L2CAP_COC_MPS (MYNEWT_VAL_MSYS_1_BLOCK_SIZE-8)

/*  Enables LE Enhanced CoC mode.
    restrictions:
            - '(BLE_L2CAP_COC_MAX_NUM > 0) && (BLE_VERSION >= 52) if 1'*/
#define MYNEWT_VAL_BLE_L2CAP_ENHANCED_COC (0)

/*Whether to collapse incoming L2CAP fragments into a minimal set of
            mbufs.
                1: Slower, more memory efficient.
                0: Faster, less memory efficient.*/
#define MYNEWT_VAL_BLE_L2CAP_JOIN_RX_FRAGS (1)

/* The number of L2CAP channels to allocate.  The default value allows
            for the signal, ATT, and SM channels for each connection.*/
#define MYNEWT_VAL_BLE_L2CAP_MAX_CHANS (3*MYNEWT_VAL_BLE_MAX_CONNECTIONS)

/*Expiry time for incoming data packets (ms).  If this much time
            passes since the previous fragment was received, the connection is
            terminated.  A value of 0 means no timeout.*/
#define MYNEWT_VAL_BLE_L2CAP_RX_FRAG_TIMEOUT (30000)

/*The maximum number of concurrent L2CAP signal procedures.*/
#define MYNEWT_VAL_BLE_L2CAP_SIG_MAX_PROCS (1)


#define MYNEWT_VAL_BLE_MESH (0)




/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//////////////////////////////  BLE host monitor ////////////////

/*Size of internal buffer for console output. Any line exceeding this
            length value will be split.*/
#define MYNEWT_VAL_BLE_MONITOR_CONSOLE_BUFFER_SIZE (128)

/*Enables monitor interface over RTT*/
#define MYNEWT_VAL_BLE_MONITOR_RTT (0)

/*   Enables buffering when using monitor interface over RTT. The data
            are written to RTT once complete packet is created in intermediate
            buffer. This allows to skip complete packet if there is not enough
            space in RTT buffer (e.g. there is no reader connected). If disabled,
            monitor will simply block waiting for RTT to free space in buffer.*/
#define MYNEWT_VAL_BLE_MONITOR_RTT_BUFFERED (0)


#define MYNEWT_VAL_BLE_MONITOR_RTT_BUFFER_NAME ("btmonitor")

/* Monitor interface upstream buffer size*/
#define MYNEWT_VAL_BLE_MONITOR_RTT_BUFFER_SIZE (256)



/* Enables monitor interface over UART*/
#define MYNEWT_VAL_BLE_MONITOR_UART (0)


#define MYNEWT_VAL_BLE_MONITOR_UART_BAUDRATE (1000000)

/* Monitor interface ringbuffer size for UART.
            This value should be a power of 2.*/
#define MYNEWT_VAL_BLE_MONITOR_UART_BUFFER_SIZE (64)


#define MYNEWT_VAL_BLE_MONITOR_UART_DEV ("uart0")

/*   The rate that new random addresses should be generated (seconds).*/
#define MYNEWT_VAL_BLE_RPA_TIMEOUT (300)




///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// the security manager protocol of the nimble Host 
////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


/*  Enables bonding (persistence and restoration of secure links). (0/1)*/
#define MYNEWT_VAL_BLE_SM_BONDING (0)

/*The IO capabilities to report during pairing.  Valid values are:
                BLE_HS_IO_DISPLAY_ONLY
                BLE_HS_IO_DISPLAY_YESNO
                BLE_HS_IO_KEYBOARD_ONLY
                BLE_HS_IO_NO_INPUT_OUTPUT
                BLE_HS_IO_KEYBOARD_DISPLAY*/
#define BLE_HS_IO_NO_INPUT_OUTPUT 0x03

#define MYNEWT_VAL_BLE_SM_IO_CAP  (BLE_HS_IO_NO_INPUT_OUTPUT)

/*Whether keypress support is advertised during pairing. (0/1)*/
#define MYNEWT_VAL_BLE_SM_KEYPRESS (0)

/*'Security manager legacy pairing.'*/
#define MYNEWT_VAL_BLE_SM_LEGACY (1)

/*The maximum number of concurrent security manager procedures.*/
#define MYNEWT_VAL_BLE_SM_MAX_PROCS (1)

/* Whether man-in-the-middle protection is advertised during
            pairing. (0/1)*/
#define MYNEWT_VAL_BLE_SM_MITM (0)

/*   Whether the out-of-band pairing algorithm is advertised. (0/1)*/
#define MYNEWT_VAL_BLE_SM_OOB_DATA_FLAG (0)

/*  A bitmap indicating which keys to distribute during pairing.  The
            bits are defined as follows:
                0x01: BLE_SM_PAIR_KEY_DIST_ENC
                0x02: BLE_SM_PAIR_KEY_DIST_ID
                0x04: BLE_SM_PAIR_KEY_DIST_SIGN
                0x08: BLE_SM_PAIR_KEY_DIST_LINK*/
#define MYNEWT_VAL_BLE_SM_OUR_KEY_DIST (0)

/* 'Security manager secure connections (4.2).'*/
#define MYNEWT_VAL_BLE_SM_SC (0)

/*   Enable SM debug mode. In this mode SM uses predefined DH key pair as
            described in Core Specification 5.0, Vol. 3, Part H, 2.3.5.6.1. This
            allows to decrypt air traffic easily and thus should be only used
            for debugging.*/
#define MYNEWT_VAL_BLE_SM_SC_DEBUG_KEYS (0)

/*A bitmap indicating which keys to accept during pairing.  The
            bits are defined as follows:
                0x01: BLE_SM_PAIR_KEY_DIST_ENC
                0x02: BLE_SM_PAIR_KEY_DIST_ID
                0x04: BLE_SM_PAIR_KEY_DIST_SIGN
                0x08: BLE_SM_PAIR_KEY_DIST_LINK*/
#define MYNEWT_VAL_BLE_SM_THEIR_KEY_DIST (0)

/*  Maximum number of bonds that can be persisted.  Note: increasing
            this value may also require increasing the capacity of the
            underlying storage mechanism.*/
#define MYNEWT_VAL_BLE_STORE_MAX_BONDS (3)

/*Maximum number of client characteristic configuration descriptors
            that can be persisted.  Note: increasing this value may also
            require increasing the capacity of the underlying storage
            mechanism.*/
#define MYNEWT_VAL_BLE_STORE_MAX_CCCDS (8)




////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/*** @apache-mynewt-nimble/nimble/host/services/ans */

#define MYNEWT_VAL_BLE_SVC_ANS_NEW_ALERT_CAT (0)


#define MYNEWT_VAL_BLE_SVC_ANS_SYSINIT_STAGE (303)


#define MYNEWT_VAL_BLE_SVC_ANS_UNR_ALERT_CAT (0)

/*** @apache-mynewt-nimble/nimble/host/services/bas */

#define MYNEWT_VAL_BLE_SVC_BAS_BATTERY_LEVEL_NOTIFY_ENABLE (1)


#define MYNEWT_VAL_BLE_SVC_BAS_BATTERY_LEVEL_READ_PERM (1)


#define MYNEWT_VAL_BLE_SVC_BAS_SYSINIT_STAGE (303)




/*

-------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------
----------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
-----------------------------------------------   BLE host services ----------------------------------



*/



/*** @apache-mynewt-nimble/nimble/host/services/dis */

#define MYNEWT_VAL_BLE_SVC_DIS_DEFAULT_READ_PERM (-1)


#define MYNEWT_VAL_BLE_SVC_DIS_FIRMWARE_REVISION_DEFAULT (NULL)

/* Value copied from BLE_SVC_DIS_DEFAULT_READ_PERM */

#define MYNEWT_VAL_BLE_SVC_DIS_FIRMWARE_REVISION_READ_PERM (-1)


#define MYNEWT_VAL_BLE_SVC_DIS_HARDWARE_REVISION_DEFAULT (NULL)

/* Value copied from BLE_SVC_DIS_DEFAULT_READ_PERM */

#define MYNEWT_VAL_BLE_SVC_DIS_HARDWARE_REVISION_READ_PERM (-1)


#define MYNEWT_VAL_BLE_SVC_DIS_MANUFACTURER_NAME_DEFAULT (NULL)

/* Value copied from BLE_SVC_DIS_DEFAULT_READ_PERM */

#define MYNEWT_VAL_BLE_SVC_DIS_MANUFACTURER_NAME_READ_PERM (-1)


#define MYNEWT_VAL_BLE_SVC_DIS_MODEL_NUMBER_DEFAULT ("Apache Mynewt NimBLE")


#define MYNEWT_VAL_BLE_SVC_DIS_MODEL_NUMBER_READ_PERM (0)


#define MYNEWT_VAL_BLE_SVC_DIS_SERIAL_NUMBER_DEFAULT (NULL)

/* Value copied from BLE_SVC_DIS_DEFAULT_READ_PERM */

#define MYNEWT_VAL_BLE_SVC_DIS_SERIAL_NUMBER_READ_PERM (-1)


#define MYNEWT_VAL_BLE_SVC_DIS_SOFTWARE_REVISION_DEFAULT (NULL)

/* Value copied from BLE_SVC_DIS_DEFAULT_READ_PERM */

#define MYNEWT_VAL_BLE_SVC_DIS_SOFTWARE_REVISION_READ_PERM (-1)


#define MYNEWT_VAL_BLE_SVC_DIS_SYSINIT_STAGE (303)


#define MYNEWT_VAL_BLE_SVC_DIS_SYSTEM_ID_DEFAULT (NULL)

/* Value copied from BLE_SVC_DIS_DEFAULT_READ_PERM */

#define MYNEWT_VAL_BLE_SVC_DIS_SYSTEM_ID_READ_PERM (-1)



/*** @apache-mynewt-nimble/nimble/host/services/gatt */

#define MYNEWT_VAL_BLE_SVC_GATT_SYSINIT_STAGE (302)

/*** @apache-mynewt-nimble/nimble/host/services/ias */

#define MYNEWT_VAL_BLE_SVC_IAS_SYSINIT_STAGE (303)

/*** @apache-mynewt-nimble/nimble/host/services/ipss */

#define MYNEWT_VAL_BLE_SVC_IPSS_SYSINIT_STAGE (303)

/*** @apache-mynewt-nimble/nimble/host/services/lls */

#define MYNEWT_VAL_BLE_SVC_LLS_SYSINIT_STAGE (303)

/*** @apache-mynewt-nimble/nimble/host/services/tps */

#define MYNEWT_VAL_BLE_SVC_TPS_SYSINIT_STAGE (303)



////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////// ble hci ram transport layer for combined host + controller 
//////////////////////////////////////////////////////////////////////////////////////////////////////


// /*** @apache-mynewt-nimble/nimble/transport/socket */


/*Number of high-priority event buffers.*/
#define MYNEWT_VAL_BLE_HCI_EVT_HI_BUF_COUNT (30)

/*Number of low-priority event buffers*/
#define MYNEWT_VAL_BLE_HCI_EVT_LO_BUF_COUNT (8)


/*'The number of ACL data buffers'*/
#define MYNEWT_VAL_BLE_ACL_BUF_COUNT (24)

/*  This is the maximum size of the data portion of HCI ACL data
            packets. It does not include the HCI data header (of 4 bytes).*/
#define MYNEWT_VAL_BLE_ACL_BUF_SIZE (255)


// #define MYNEWT_VAL_BLE_HCI_ACL_OUT_COUNT (12)

/* 'Size of each event buffer, in bytes.'*/
#define MYNEWT_VAL_BLE_HCI_EVT_BUF_SIZE (70)







// #define MYNEWT_VAL_BLE_SOCK_CLI_SYSINIT_STAGE (500)


// #define MYNEWT_VAL_BLE_SOCK_LINUX_DEV (0)


// #define MYNEWT_VAL_BLE_SOCK_STACK_SIZE (80)


// #define MYNEWT_VAL_BLE_SOCK_TASK_PRIO (9)


// #define MYNEWT_VAL_BLE_SOCK_TCP_PORT (14433)


// #define MYNEWT_VAL_BLE_SOCK_USE_LINUX_BLUE (0)


// #define MYNEWT_VAL_BLE_SOCK_USE_TCP (1)

// /*** newt */

// #define MYNEWT_VAL_APP_NAME ("dummy_app")


// #define MYNEWT_VAL_APP_dummy_app (1)


// #define MYNEWT_VAL_ARCH_NAME ("dummy")


// #define MYNEWT_VAL_ARCH_dummy (1)


// #define MYNEWT_VAL_BSP_NAME ("dummy_bsp")


// #define MYNEWT_VAL_BSP_dummy_bsp (1)


// #define MYNEWT_VAL_NEWT_FEATURE_LOGCFG (0)


// #define MYNEWT_VAL_NEWT_FEATURE_SYSDOWN (1)


// #define MYNEWT_VAL_TARGET_NAME ("porting_default")


// #define MYNEWT_VAL_TARGET_porting_default (1)




/*
================================================================================
========================================================================================
========================== BLe host mesh architecture ==================================
*/

// #define MYNEWT_VAL_BLE_MESH_ADV_TASK_PRIO (9)


// /*** @apache-mynewt-nimble/nimble/host/mesh */
// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_ADV_BUF_COUNT (20)


// #define MYNEWT_VAL_BLE_MESH_APP_KEY_COUNT (1)


// #define MYNEWT_VAL_BLE_MESH_CFG_CLI (0)

// #define MYNEWT_VAL_BLE_MESH_CRPL (10)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */


// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

#define MYNEWT_VAL_BLE_MESH_DEBUG_ACCESS (0)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_ADV (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

#define MYNEWT_VAL_BLE_MESH_DEBUG (0)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_ACCESS (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_ADV (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_BEACON (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_CRYPTO (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_FRIEND (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_LOW_POWER (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_MODEL (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_NET (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_PROV (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_PROXY (1)


// #define MYNEWT_VAL_BLE_MESH_DEBUG_SETTINGS (1)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_DEBUG_TRANS (1)


// #define MYNEWT_VAL_BLE_MESH_DEVICE_NAME ("nimble-mesh-node")


// #define MYNEWT_VAL_BLE_MESH_DEV_UUID (((uint8_t[16]){0x11, 0x22, 0}))


// #define MYNEWT_VAL_BLE_MESH_FRIEND (0)


// #define MYNEWT_VAL_BLE_MESH_FRIEND_LPN_COUNT (2)


// #define MYNEWT_VAL_BLE_MESH_FRIEND_QUEUE_SIZE (16)


// #define MYNEWT_VAL_BLE_MESH_FRIEND_RECV_WIN (255)


// #define MYNEWT_VAL_BLE_MESH_FRIEND_SEG_RX (1)


// #define MYNEWT_VAL_BLE_MESH_FRIEND_SUB_LIST_SIZE (3)


// #define MYNEWT_VAL_BLE_MESH_GATT_PROXY (1)


// #define MYNEWT_VAL_BLE_MESH_HEALTH_CLI (0)


// #define MYNEWT_VAL_BLE_MESH_IVU_DIVIDER (4)


// #define MYNEWT_VAL_BLE_MESH_IV_UPDATE_TEST (0)


// #define MYNEWT_VAL_BLE_MESH_LABEL_COUNT (1)


// #define MYNEWT_VAL_BLE_MESH_LOW_POWER (0)


// #define MYNEWT_VAL_BLE_MESH_LPN_AUTO (1)


// #define MYNEWT_VAL_BLE_MESH_LPN_AUTO_TIMEOUT (15)


// #define MYNEWT_VAL_BLE_MESH_LPN_ESTABLISHMENT (1)


// #define MYNEWT_VAL_BLE_MESH_LPN_GROUPS (10)


// #define MYNEWT_VAL_BLE_MESH_LPN_INIT_POLL_TIMEOUT (MYNEWT_VAL_BLE_MESH_LPN_POLL_TIMEOUT)


// #define MYNEWT_VAL_BLE_MESH_LPN_MIN_QUEUE_SIZE (1)


// #define MYNEWT_VAL_BLE_MESH_LPN_POLL_TIMEOUT (300)


// #define MYNEWT_VAL_BLE_MESH_LPN_RECV_DELAY (100)


// #define MYNEWT_VAL_BLE_MESH_LPN_RECV_WIN_FACTOR (0)


// #define MYNEWT_VAL_BLE_MESH_LPN_RETRY_TIMEOUT (8)


// #define MYNEWT_VAL_BLE_MESH_LPN_RSSI_FACTOR (0)


// #define MYNEWT_VAL_BLE_MESH_LPN_SCAN_LATENCY (10)


// #define MYNEWT_VAL_BLE_MESH_MODEL_GROUP_COUNT (1)


// #define MYNEWT_VAL_BLE_MESH_MODEL_KEY_COUNT (1)


// #define MYNEWT_VAL_BLE_MESH_MSG_CACHE_SIZE (10)


// #define MYNEWT_VAL_BLE_MESH_NODE_ID_TIMEOUT (60)


// #define MYNEWT_VAL_BLE_MESH_OOB_INPUT_ACTIONS (((BT_MESH_NO_INPUT)))


// #define MYNEWT_VAL_BLE_MESH_OOB_INPUT_SIZE (4)


// #define MYNEWT_VAL_BLE_MESH_OOB_OUTPUT_ACTIONS (((BT_MESH_DISPLAY_NUMBER)))


// #define MYNEWT_VAL_BLE_MESH_OOB_OUTPUT_SIZE (4)


// #define MYNEWT_VAL_BLE_MESH_PB_ADV (1)


// #define MYNEWT_VAL_BLE_MESH_PB_GATT (1)

// /* Overridden by @apache-mynewt-nimble/nimble/host/mesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_PROV (1)

// /* Overridden by @apache-mynewt-nimble/nimble/host/mesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_PROXY (1)


// #define MYNEWT_VAL_BLE_MESH_PROXY_FILTER_SIZE (1)


// #define MYNEWT_VAL_BLE_MESH_RELAY (0)


// #define MYNEWT_VAL_BLE_MESH_RPL_STORE_TIMEOUT (5)


// #define MYNEWT_VAL_BLE_MESH_RX_SDU_MAX (72)


// #define MYNEWT_VAL_BLE_MESH_RX_SEG_MSG_COUNT (2)


// #define MYNEWT_VAL_BLE_MESH_SEQ_STORE_RATE (128)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_SETTINGS (0)


// #define MYNEWT_VAL_BLE_MESH_SHELL (0)


// #define MYNEWT_VAL_BLE_MESH_SHELL_MODELS (0)


// #define MYNEWT_VAL_BLE_MESH_STORE_TIMEOUT (2)


// #define MYNEWT_VAL_BLE_MESH_SUBNET_COUNT (1)


// #define MYNEWT_VAL_BLE_MESH_TESTING (0)

// /* Overridden by apps/blemesh (defined by @apache-mynewt-nimble/nimble/host/mesh) */

// #define MYNEWT_VAL_BLE_MESH_TX_SEG_MAX (6)


// #define MYNEWT_VAL_BLE_MESH_TX_SEG_MSG_COUNT (4)




#endif


