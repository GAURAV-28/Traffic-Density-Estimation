#!/bin/bash
for i in {1..16}
do  
    printf "`./output trafficvideo "$i"` `python3 compare.py "M1output/output_$i.txt" M1output/std_out.txt`\n" 
done > M1output/err.txt