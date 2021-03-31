#!/bin/bash
for i in $(seq 0.05 0.15 0.95)
do  
    printf "`./output trafficvideo "$i"` `python3 compare.py "M2output/out_with_factor_"$i"0000.txt" M2output/std_out.txt` `echo "$i"`\n" 
done > err.txt

printf "`./output trafficvideo 1` `python3 compare.py "M2output/out_with_factor_1.000000.txt" M2output/std_out.txt` `echo 1.00`\n" >> err.txt 

/usr/bin/gnuplot <<__EOF
set xlabel "Run Time"
set ylabel "Utility"
set title "Runtime-Utility Analysis \n Method 2 \n (f = Resolution Factor)"
set term png
set output "plot.png"
set arrow from 78, graph 0 to 78, graph 1 nohead lc rgb "red" lw 2
plot "err.txt" u 1:2 w l lw 2 lc rgb "blue" notitle , '' u 1:2:3 with labels point pt 7 lc 2 lw 5 offset 0.4,0.4 title "" 
pause -1
__EOF