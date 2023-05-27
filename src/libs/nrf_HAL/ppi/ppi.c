#include "ppi.h"

/// @brief used to add task and events to a particular channel number
/// @param  channel_number
/// @param  Eaddress of the event 
/// @param  Taddress of task
void ppi_configure_channel(uint8_t ch_no , uint32_t * evnt, uint32_t * tsk)
{
    // first dsiable the channel
    NRF_PPI->CHENCLR = _BV(ch_no);
    // protect guard if ch_no > 20
    if(ch_no > 20) return;
    // check if the addr are null
    if((evnt == NULL) || (tsk == NULL))return;

    NRF_PPI->CH[ch_no].EEP = (uint32_t)evnt;
    NRF_PPI->CH[ch_no].TEP = (uint32_t)tsk;

}


void ppi_add_fork_task_channel(uint8_t ch_no, uint32_t * tsk)
{
        // first dsiable the channel
    NRF_PPI->CHENCLR = _BV(ch_no);
    if(tsk == NULL) return;
    NRF_PPI->FORK[ch_no].TEP = (uint32_t)tsk;
}


void ppi_init(void)
{
    // clear all the group register 
    for(uint8_t i=0; i<6; i++)
    {
        ppi_group_clear(i);
    }
    
    // disable all the channels 
    NRF_PPI->CHENCLR = U32_MAX;
}


void ppi_deinit(void)
{
    
    // disable all the groups 
    for(uint8_t i=0; i<6; i++)
    {
    ppi_group_clear(i);
    }

    // disable all the channels 
    NRF_PPI->CHENCLR = U32_MAX;

}


void ppi_add_cahnnel_group(uint8_t  group_no ,uint8_t ch_no )
{
 if(group_no > 5)return;
    NRF_PPI->CHG[group_no] |= _BV(ch_no);
}

 
void ppi_group_clear(uint8_t group_no)
{
    if(group_no > 5)return;
    NRF_PPI->CHG[group_no] = U_0;
}
 
void ppi_group_enable(uint8_t group_no )
{
    if(group_no > 5)return;
    NRF_PPI->TASKS_CHG[group_no].EN = 1UL;
}


 
void ppi_group_disable(uint8_t group_no )
{
   if(group_no > 5)return;
    NRF_PPI->TASKS_CHG[group_no].DIS = 1UL;
}

 
void ppi_channel_enable(uint8_t ch_no)
{
    if(ch_no > 31)return;
    NRF_PPI->CHENSET = _BV(ch_no);
}

 
void ppi_channel_disable(uint8_t ch_no )
{
    if(ch_no > 31)return;
    NRF_PPI->CHENCLR = _BV(ch_no);
}




