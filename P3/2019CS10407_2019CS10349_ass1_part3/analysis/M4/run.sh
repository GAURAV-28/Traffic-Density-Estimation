#!/bin/bash
for i in $(seq 2 1 8)
do  
    printf "`./m4_output "$1" "$i"` `python3 compare.py "M4output/m4_num_$i.txt" M4output/output.txt` `echo "N$i"` `echo "$i"`\n" 
done > err.txt

/usr/bin/gnuplot <<__EOF
set xlabel "Run Time"
set ylabel "Utility"
set title "Utility Run Time Analysis \n Method 4 \n N = No of Threads"
set yrange [0.99:1]
set term png
set output "plot.png"
set arrow from "$2", graph 0 to "$2", graph 1 nohead lc rgb "red" lw 2
plot "err.txt" u 1:2:3 with labels point pt 7 lc 2 lw 5 offset 0.4,0.4 title "" 
pause -1
set term png
set output "plot1.png"
set xlabel "No of threads"
set ylabel "Run Time"
set xrange [0:10]
unset yrange
plot "err.txt" u 4:1 w lp pt 7 lw 2 lc rgb "blue" notitle , "$2" lc rgb "red" lw 2 notitle 
__EOF