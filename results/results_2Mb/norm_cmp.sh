#!/bin/bash

make_comp() {
	grep accuracy $1 | sed 's/accuracy/minmax/'  >  temp1
	grep accuracy $2 | sed 's/accuracy/stdnorm/' >>  temp1
	title=${3//_/,}
	sort temp1 > temp2
	sed 's/DATAFILE/temp2/' gn_cmp.plt | sed "s/TITLE/$title/" | sed "s/OUTFILE/norm_$3.eps/" | gnuplot 
	rm temp1 temp2
}

make_comp test_delay_2Mb_minmax_100.tr  test_delay_2Mb_stdnorm_100.tr delay_2Mb_100
make_comp test_delay_2Mb_minmax_50.tr   test_delay_2Mb_stdnorm_50.tr  delay_2Mb_50

make_comp test_delayvar_2Mb_minmax_100.tr test_delayvar_2Mb_stdnorm_100.tr delayvar_2Mb_100
make_comp test_delayvar_2Mb_minmax_50.tr test_delayvar_2Mb_stdnorm_50.tr delayvar_2Mb_50

make_comp test_nodelay_2Mb_minmax_100.tr test_nodelay_2Mb_stdnorm_100.tr  nodelay_2Mb_100
make_comp test_nodelay_2Mb_minmax_50.tr  test_nodelay_2Mb_stdnorm_50.tr   nodelay_2Mb_50


gs -sDEVICE=pdfwrite -dNOPAUSE -dBATCH -dSAFER -sOutputFile=results.pdf norm*.eps
