#!/bin/bash

executable_path=$(locate -n 1 -b '\SOP_synthesis')
test_result_filename="test_tmp_results"

for ((i=0; i<=20; i+=1)); do
	/usr/bin/time -a -o $test_result_filename -p $executable_path "sopp_time" 4
done


