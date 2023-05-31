#ifndef PPI_H_
#define PPI_H_

#include "system.h"



/// @brief / define the channels here 
enum _PPI_CHANNEL_TYPES_
{
    PPI_CHANNEL_0 =0,
    PPI_CHANNEL_1, 
    PPI_CHANNEL_2,
    PPI_CHANNEL_3,
    PPI_CHANNEL_4,
    PPI_CHANNEL_5,
    PPI_CHANNEL_6,
    PPI_CHANNEL_7,
    PPI_CHANNEL_8,
    PPI_CHANNEL_9,
    PPI_CHANNEL_10,
    PPI_CHANNEL_11,
    PPI_CHANNEL_12,
    PPI_CHANNEL_13,
    PPI_CHANNEL_14,
    PPI_CHANNEL_15,
    PPI_CHANNEL_16,
    PPI_CHANNEL_17,
    PPI_CHANNEL_18,
    PPI_CHANNEL_19,
    PPI_CHANNEL_20,
    PPI_CHANNEL_21,
    PPI_CHANNEL_22,
    PPI_CHANNEL_23,
    PPI_CHANNEL_24,
    PPI_CHANNEL_25,
    PPI_CHANNEL_26,
    PPI_CHANNEL_27,
    PPI_CHANNEL_28,
    PPI_CHANNEL_29,
    PPI_CHANNEL_30,
    PPI_CHANNEL_31,

};

enum _PPI_GROUP_TYPES_
{
    PPI_GROUP_0 =0,
    PPI_GROUP_1,
    PPI_GROUP_2,
    PPI_GROUP_3,
    PPI_GROUP_4,
    PPI_GROUP_5,
    PPI_GROUP_6
};


/*****
 * @name ppi_configure_channel
 * @param[1] the channel number 
 * @param[2] the address of the event 
 * @param[3] the address of the task
 * @brief used to connect an event and task through the channel number 
*/
void ppi_configure_channel(uint8_t , uint32_t *, uint32_t *);

/*****
 * @name ppi_configure_channel
 * @param[1] the channel number 
 * @param[2] the address of the event 
 * @param[3] the address of the task
 * @brief used to add a secondry task to a channel
 */
void ppi_add_fork_task_channel(uint8_t, uint32_t *);

/****
 * @name  ppi_init
 * @param void
 * @brief used to init the ppi module 
 * this will clear the channel group  and disable all the enabled channel
 * */ 
void ppi_init(void);

/****
 * @name  ppi_deinit
 * @param void
 * @brief used to deinit the ppi module 
 * this will clear the channel group  and disable all the enabled channel
 * */ 
void ppi_deinit(void);


/*****
 * @name ppi_add_chaanel_group
 * @param[1] group number
 * @param[2] channel number
 * @brief used to add a channel into the group
*/
void ppi_add_cahnnel_group(uint8_t ,uint8_t);

/******
 * @name ppi_group_clear
 * @param group number 
 * @brief this is used to clear the group enteries 
*/
void ppi_group_clear(uint8_t );

/****
 * @name ppi_group_enable
 * @param group_number 
 * @brief used to enable a group
*/
void ppi_group_enable(uint8_t);


/****
 * @name ppi_group_disable
 * @param group_number 
 * @brief used to disable a group
*/
void ppi_group_disable(uint8_t);

/****
 * @name ppi_channel_enable
 * @param channel number
 * @brief enable a channel of the PPI
*/
void ppi_channel_enable(uint8_t );

/******
 * @name ppi_channel_disable
 * @param channel_number 
 * @brief dissable a channel of the PPI
*/
void ppi_channel_disable(uint8_t);








#endif