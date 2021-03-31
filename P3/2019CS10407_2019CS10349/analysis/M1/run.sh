#!/bin/bash
for i in {1..10}
do  
    printf "`./m1_output "$1" "$i"` `python3 compare.py "M1output/output_$i.txt" M1output/std_out.txt` `echo "x=$i"`\n" 
done > err.txt 

/usr/bin/gnuplot <<__EOF
set xlabel "Run Time"
set ylabel "Utility"
set title "Runtime-Utility Tradeoff Analysis \n Method 1 \n (N+x frame is processed after N frame)"
set term png
set output "plot.png"
set arrow from "$2", graph 0 to "$2", graph 1 nohead lc rgb "red" lw 2
plot "err.txt" u 1:2 w l lc rgb "blue" lw 2 title "", '' u 1:2:3 with labels point pt 7 lc 1  lw 5 offset 1.9,-0.5 title ""
pause -1
__EOF