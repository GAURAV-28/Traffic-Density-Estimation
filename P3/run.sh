#!/bin/bash
for i in $(seq 1 1 8)
do  
    ./output trafficvideo "$i"
done

#/usr/bin/gnuplot <<__EOF
#set xlabel "Run Time"
#set ylabel "Error"
#set title "Method 2 Analysis \n (x = Resolution Factor)"
#set term png
#set output "plot.png"
#plot "err.txt" u 1:2 w l lc 7 lw 2 title "", '' u 1:2:3 with labels point pt 11 offset 2.1,0.3 title "factor x"
#pause -1
#__EOF