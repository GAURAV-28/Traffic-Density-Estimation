import sys
import math

filename1 = sys.argv[1]
filename2 = sys.argv[2]


file1 = open(filename1).readlines() 
 
file1_line = [] 
 
for lines in file1: 
 file1_line.append(lines) 
 
file2 = open(filename2).readlines() 
 
file2_line = [] 
 
for lines in file2: 
 file2_line.append(lines) 
 
if len(file1) > len(file2): 
 print("Length Of File of ",filename1,"is greater than",filename2,len(file1),">",len(file2)) 
 
elif len(file1) < len(file2): 
 print("Length Of File of ",filename1,"is less than",filename2,len(file1),"<",len(file2)) 
 
else: 
 n = 0 
 error = 0.0
 sum = 0.0
 for line in file1_line: 
   l = line.split(" ")
   m = file2_line[n].split(" ")
   k = (float(l[1])-float(m[1]))
   error += k*k
   sum += float(m[1])
   n += 1 
 avg = sum/n
 err = math.sqrt(error/n)
 print((avg - err)/avg)
