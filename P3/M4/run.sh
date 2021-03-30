#!/bin/bash
for i in $(seq 1 1 8)
do  
    printf "`./output trafficvideo "$i"` `echo "$i"`\n" 
done > err.txt

/usr/bin/gnuplot <<__EOF
set yrange [0:10]
set xlabel "Run Time"
set ylabel "No of Threads"
set title "Method 4 Analysis"
set term png
set output "plot.png"
plot "err.txt" u 1:2 w l lc 7 lw 2 title ""
pause -1
__EOF