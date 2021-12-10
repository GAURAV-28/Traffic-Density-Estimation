# Utility-Runtime Trade-Off Analysis
For utility-runtime trade-off analysis, we have the following methods/parameters:

## Method 1: 
[[code]](./M1/code/) [[analysis]](./M1/analysis/)

Sub-sampling frames -- processing every x frame i.e. process frame N and then frame N+x, and for all intermediate frames just use the value obtained for N - total processing time will reduce, but utility might decrease as intermediate frames values might differ from baseline. Parameter for this method is x, how many frames you drop. 


## Method 2: 
[[code]](./M2/code/) [[analysis]](./M2/analysis/)

Reduce resolution for each frame. Lower resolution frames might be processed faster, but having higher errors. Parameter can be resolution XxY. 



## Method 3: 
[[code]](./M3/code/) [[analysis]](./M3/analysis/)

Split work spatially across threads (application level pthreads) by giving each thread part of a frame to process. Parameter can be number of threads, if each thread gets one split. 


## Method 4: 
[[code]](./M4/code/) [[analysis]](./M4/analysis/)

Split work temporally across threads (application level pthreads), by giving consecutive frames to different threads for processing. Parameter can be number of threads. 
