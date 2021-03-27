#!/bin/bash
for i in {1..15}
do  
    printf "`./output trafficvideo "$i"` `python3 compare.py "M1output/output_$i.txt" M1output/std_out.txt` `echo "x=$i"`\n" 
done > err.txt 

/usr/bin/gnuplot <<__EOF
set xlabel "Run Time"
set ylabel "Error"
set title "Method 1 Analysis \n (x = No of frames between two consecutive samples)"
set term png
set output "plot.png"
plot "err.txt" u 1:2 w l lc 7 lw 2 title "", '' u 1:2:3 with labels point pt 11 offset 2.1,0.3 title "x"
pause -1
__EOF