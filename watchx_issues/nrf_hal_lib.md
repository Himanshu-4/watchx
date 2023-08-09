### nordic Hal Lib 



## nvs library issue 
- the nvs library have to store the data in ram in modifying and deleting data of nvs
- but in that function we are using the local task stack . so we can trigger the stack overflow error in task local stack 
- because the function have a limited memory that is allocated for the  task at the time of its creation  
- we should use the malloc and free . but for now we can assume that nvs data cannt be very big in size 
- the max we are using nvs data is 500 bytes . so for that context we are for now using task local stack 
