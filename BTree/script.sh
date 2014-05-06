#! /bin/sh

# 1 thread read
./nocbt tst.db f 14 32768 4 0 skew1_1 &> result/1f_noc

# 2 threads read
./lbt tst.db f 14 32768 4 0 skew1_1 skew2_1 &> result/2f_l
./tsxbt tst.db f 14 32768 4 0 skew1_1 skew2_1 &> result/2f_tsx
./nocbt tst.db f 14 32768 4 0 skew1_1 skew2_1 &> result/2f_noc

# 4 threads read
./lbt tst.db f 14 32768 4 0 skew1_1 skew2_1 skew3_1 skew4_1 &> result/4f_l
./tsxbt tst.db f 14 32768 4 0 skew1_1 skew2_1 skew3_1 skew4_1 &> result/4f_tsx
./nocbt tst.db f 14 32768 4 0 skew1_1 skew2_1 skew3_1 skew4_1 &> result/4f_noc

rm tst.db

./nocbt tst.db w 14 32768 4 0 skew1_1
# 1 thread read
./lbt tst.db f 14 32768 4 0 skew1_1 &> result/1f_ls
./tsxbt tst.db f 14 32768 4 0 skew1_1 &> result/1f_tsxs
./nocbt tst.db f 14 32768 4 0 skew1_1 &> result/1f_nocs

# 2 threads read
./lbt tst.db f 14 32768 4 0 skew1_1 skew2_1 &> result/2f_ls
./tsxbt tst.db f 14 32768 4 0 skew1_1 skew2_1 &> result/2f_tsxs
./nocbt tst.db f 14 32768 4 0 skew1_1 skew2_1 &> result/2f_nocs

# 4 threads read
./lbt tst.db f 14 32768 4 0 skew1_1 skew2_1 skew3_1 skew4_1 &> result/4f_ls
./tsxbt tst.db f 14 32768 4 0 skew1_1 skew2_1 skew3_1 skew4_1 &> result/4f_tsxs
./nocbt tst.db f 14 32768 4 0 skew1_1 skew2_1 skew3_1 skew4_1 &> result/4f_nocs



