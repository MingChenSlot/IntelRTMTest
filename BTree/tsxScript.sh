#########################################################################
# File Name: writeScript.sh
# Author: chen ming
# mail: mchen67@wisc.edu
# Created Time: Sat 03 May 2014 03:21:55 PM CDT
#########################################################################
#!/bin/bash
make clean
make

# 1 thread insert
./tsxbt tst.db w 14 32768 4 0 distinct_1 &> result/1w_tsx

# 1 thread read
./tsxbt tst.db f 14 32768 4 0 skew1_1 &> result/1f_tsx

# 2 threads read
./tsxbt tst.db f 14 32768 4 0 skew1_1 skew2_1 &> result/2f_tsx

# 4 threads read
./tsxbt tst.db f 14 32768 4 0 skew1_1 skew2_1 skew3_1 skew4_1 &> result/4f_tsx
rm tst.db

# 2 threads insert
./tsxbt tst.db w 14 32768 4 0 skew1_1 skew2_1 &> result/2w_tsx
rm tst.db

# 4 threads insert
#./nocbt tst.db w 14 32768 4 0 skew1_1 skew2_1 skew3_1 skew4_1 &> result/4w_noc
#rm tst.db

#./lbt tst.db w 14 32768 4 0 skew1_1 skew2_1 skew3_1 skew4_1 &> result/4w_l
#rm tst.db

./tsxbt tst.db w 14 32768 4 0 skew1_1 skew2_1 skew3_1 skew4_1 &> result/4w_tsx
rm tst.db



