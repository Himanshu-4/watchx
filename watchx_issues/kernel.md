### kernel device driver issue 







### kernel mem manager issue 

1-> 
- when using the pvportmalloc function the ram consumption increases  almost twice
- so for now on only using malloc , and free functions   

- for now ownwards we are creating and managing memory on the basis of array , and for removing  and modifying the content we are copying the rest of the data after the intended index . this is the worst case . because we have to do copy the stuff twice . 
// in the future version we are using the blocks memory implementation . so these blcoks can be connected through the link list type methods .


### kernel time manager libaray 
- when we are initing the kernel time module which actually init the rtc peripheral , after some time procesoor get hangged . have to figure out 
- also watchdog is not running at all

- processor got hangged at some intervals s

- |||||Solved|||||  -> the interrupt should be cleared after getting the interrupt otherwise that interrupt triger again anf again  
************************************************
**************************************************************************************************** 

