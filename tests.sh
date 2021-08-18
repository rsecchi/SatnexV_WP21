#!/bin/bash

run_test() {
	NORM_TYPE=$1 ./run_ML_test.sh 100 $3 results/$2 > results/$2_$1_$3.tr
	mv GT.csv results/GT_$2_$1.csv
}

run_test stdnorm test_nodelay_5Mb 100 &
run_test minmax  test_nodelay_5Mb 100 &
run_test stdnorm test_nodelay_5Mb 50  &
run_test minmax  test_nodelay_5Mb 50  &

wait

run_test stdnorm test_delay_5Mb 100 &
run_test minmax  test_delay_5Mb 100 &
run_test stdnorm test_delay_5Mb 50  &
run_test minmax  test_delay_5Mb 50  &

wait

run_test stdnorm test_delayvar_5Mb 100 &
run_test minmax  test_delayvar_5Mb 100 &
run_test stdnorm test_delayvar_5Mb 50  &
run_test minmax  test_delayvar_5Mb 50  &

wait
