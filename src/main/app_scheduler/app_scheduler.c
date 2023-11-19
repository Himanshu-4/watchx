#include "app_scheduler.h"

///// include the kernel q lib for app schd opertaions 
#include "kernel_queue.h"


// /// @brief pre init the kernel app scheduler 
// /// @param  void 
// void app_schd_preinit(void);

// /// @brief used to get the memory usage of the current app
// /// @param  void 
// /// @return mem usage in bytes 
// uint32_t app_schd_get_mem_usage(void);

// /// @brief used to switch the app i.e. call the scheduler to run
// /// @param  void
// /// @return status of the cmd 
// uint32_t app_schd_switch(void);

// /// @brief used to lock the scheduler 
// /// @param  void
// /// @return status of the cmd
// uint32_t app_schd_lock(void);

// /// @brief used to unlcok the scheduler basically the call to app_schd_switch will result err if it is locked 
// /// @param  void 
// /// @return status 
// uint32_t app_schd_unlock(void);

// /// @brief used to get the status of the scheduler 
// /// @param  void 
// /// @return lock,unlock,etc
// uint32_t app_schd_get_status(void);

// /// @brief this is the app that will be run by the app scheduler
// /////////// this is the app type looks like 

// typedef void (*schd_app_Type)(void * param);

// typedef void  (* schd_app_pre_work)(void * param);

// typedef void (*schd_app_post_work)(void * param);

// typedef struct __packed  __APP_SCHEDULER_APP_TYPE__
// {
//     /// @brief this is the code that will be run before calling the app
//     schd_app_pre_work preinit;

//     /// @brief this is the code that actual app is running 
//     schd_app_Type actual_app;

//     /// @brief this is the code that will run after exiting from the app
//     schd_app_post_work postinit;

// }app_format;

// /// @brief send the app to the queue 
// /// @param app 
// /// @return return err code 
// uint32_t app_schd_send_app(app_format * app );


// /// @brief get the cuurently runnig app 
// /// @param  
// /// @return return the 
// app_format * app_schd_get_current_App(void);

// /// @brief get the total app in the q 
// /// @param  void 
// /// @return total app present in the q 
// uint8_t app_schd_get_total_apps_in_q(void);

// /// @brief reset the q to zero
// /// @param  void
// /// @return status of the cmd 
// uint32_t app_schd_free_q(void);

// /// @brief get the app from the q index 
// /// @param index 
// /// @return null or app 
// app_format * app_schd_get_app(uint8_t index);