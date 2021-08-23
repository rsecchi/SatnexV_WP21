#!/bin/bash

run_test() {
	DIRNAME=${2%/*}
	FULLNAME=${2##*/}
	TAG1="-$1-${2//\//-}-$3"
	TAG2="${2##*/}_$1_$3"
	OUTPUT=$DIRNAME/$TAG2.tr
	echo $TAG1
	NORM_TYPE=$1 TAG=$TAG1 ./run_ML_test.sh 100 $3 $2 > $OUTPUT
	mv GT$TAG1.csv $DIRNAME/GT_$TAG2.csv
}

# Tests single pool at 2Mb
# run_test stdnorm results/results_2Mb/test_nodelay_2Mb 100 &
# run_test minmax  results/results_2Mb/test_nodelay_2Mb 100 &
# run_test stdnorm results/results_2Mb/test_nodelay_2Mb 50 &
# run_test minmax  results/results_2Mb/test_nodelay_2Mb 50 &
# run_test stdnorm results/results_2Mb/test_delay_2Mb 100 &
# run_test minmax  results/results_2Mb/test_delay_2Mb 100 &
# run_test stdnorm results/results_2Mb/test_delay_2Mb 50 &
# run_test minmax  results/results_2Mb/test_delay_2Mb 50 &
# run_test stdnorm results/results_2Mb/test_delayvar_2Mb 100 &
# run_test minmax  results/results_2Mb/test_delayvar_2Mb 100 &
# run_test stdnorm results/results_2Mb/test_delayvar_2Mb 50 &
# run_test minmax  results/results_2Mb/test_delayvar_2Mb 50 &

# Test single pool at 5Mb
# run_test stdnorm results/results_5Mb/test_nodelay_2Mb 100 &
# run_test minmax  results/results_5Mb/test_nodelay_2Mb 100 &
# run_test stdnorm results/results_5Mb/test_nodelay_2Mb 50 &
# run_test minmax  results/results_5Mb/test_nodelay_2Mb 50 &
# run_test stdnorm results/results_5Mb/test_delay_2Mb 100 &
# run_test minmax  results/results_5Mb/test_delay_2Mb 100 &
# run_test stdnorm results/results_5Mb/test_delay_2Mb 50 &
# run_test minmax  results/results_5Mb/test_delay_2Mb 50 &
# run_test stdnorm results/results_5Mb/test_delayvar_2Mb 100 &
# run_test minmax  results/results_5Mb/test_delayvar_2Mb 100 &
# run_test stdnorm results/results_5Mb/test_delayvar_2Mb 50 &
# run_test minmax  results/results_5Mb/test_delayvar_2Mb 50 &


# Test cross checked at 2Mb
# run_test stdnorm results/results_cross_2Mb/test_cross_delay_2Mb 100 &
# run_test minmax  results/results_cross_2Mb/test_cross_delay_2Mb 100 &
# run_test stdnorm results/results_cross_2Mb/test_cross_delay_2Mb 50 &
# run_test minmax  results/results_cross_2Mb/test_cross_delay_2Mb 50 &
# run_test stdnorm results/results_cross_2Mb/test_cross_delayvar_2Mb 100 &
# run_test minmax  results/results_cross_2Mb/test_cross_delayvar_2Mb 100 &
# run_test stdnorm results/results_cross_2Mb/test_cross_delayvar_2Mb 50 &
# run_test minmax  results/results_cross_2Mb/test_cross_delayvar_2Mb 50 &

# Test cross checked at 5Mb
# run_test stdnorm results/results_cross_5Mb/test_cross_delay_5Mb 100 &
# run_test minmax  results/results_cross_5Mb/test_cross_delay_5Mb 100 &
# run_test stdnorm results/results_cross_5Mb/test_cross_delay_5Mb 50 &
# run_test minmax  results/results_cross_5Mb/test_cross_delay_5Mb 50 &
# run_test stdnorm results/results_cross_5Mb/test_cross_delayvar_5Mb 100 &
# run_test minmax  results/results_cross_5Mb/test_cross_delayvar_5Mb 100 &
# run_test stdnorm results/results_cross_5Mb/test_cross_delayvar_5Mb 50 &
# run_test minmax  results/results_cross_5Mb/test_cross_delayvar_5Mb 50 &

# Test cross checked at 5Mb --> 2Mb
run_test stdnorm results/results_cross_cap/test_cross_nodelay_5Mb 100 &
run_test minmax  results/results_cross_cap/test_cross_nodelay_5Mb 100 &
run_test stdnorm results/results_cross_cap/test_cross_nodelay_5Mb 50 &
run_test minmax  results/results_cross_cap/test_cross_nodelay_5Mb 50 &

wait

run_test stdnorm results/results_cross_cap/test_cross_delay_5Mb 100 &
run_test minmax  results/results_cross_cap/test_cross_delay_5Mb 100 &
run_test stdnorm results/results_cross_cap/test_cross_delay_5Mb 50 &
run_test minmax  results/results_cross_cap/test_cross_delay_5Mb 50 &

wait

run_test stdnorm results/results_cross_cap/test_cross_delayvar_5Mb 100 &
run_test minmax  results/results_cross_cap/test_cross_delayvar_5Mb 100 &
run_test stdnorm results/results_cross_cap/test_cross_delayvar_5Mb 50 &
run_test minmax  results/results_cross_cap/test_cross_delayvar_5Mb 50 &

wait
