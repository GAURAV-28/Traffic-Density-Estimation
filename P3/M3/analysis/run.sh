#!/bin/bash
for i in $(seq 2 1 8)
do  
    printf "`./output trafficvideo "$i"` `python3 compare.py "M3output/m3_num_$i.txt" M3output/output.txt` `echo "N$i"` `echo "$i"`\n" 
done > err.txt

/usr/bin/gnuplot <<__EOF
set xlabel "Run Time"
set ylabel "Utility"
set yrange [0.95:1.01]
set title "Runtime-Utility Tradeoff Analysis \n Method 3"
set term png
set output "plot.png"
set arrow from 78, graph 0 to 78, graph 1 nohead lc rgb "red" lw 2
plot "err.txt" u 1:2:4 with labels point pt 7 lc rgb "blue" offset 0.0,0.4 title ""
pause -1
set term png
set output "plot1.png"
set xlabel "No of threads"
set ylabel "Run Time"
set xrange [0:10]
unset yrange 
plot "err.txt" u 4:1 w lp pt 7 lw 2 lc rgb "blue" notitle , 78 lc rgb "red" lw 2 notitle 
__EOF