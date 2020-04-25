#!/bin/bash

#this script is used to compare the time of sopp and dsopp synthesis

executable_path="./cmake-build-debug/DSOPP_synthesis"
test_result_filename="test_tmp_results"
TIMEFORMAT="%5R"
n_tests=100
n_variables=6
program_out="synthesis_weights.out"
program_e_out="synthesis_e_weights.out"

#check arguments
if (($# == 0)); then
  echo Usage: bash "$0" test_type "[n_variables]" "[n_tests]"
  echo Test types aviailable: sopp dsopp
  exit
else
  test_type=$1
  if (($# > 1)); then
    n_variables=$2
    if(($# > 2)); then
      n_tests=$3
    fi
  fi
fi

#test executable existance
if ! [ -f $executable_path ]; then
  if [ $# -gt 0 ] && [ -f "$1" ]; then
    executable_path=$1
    if ! (($# > 1)); then
      echo Usage: bash "$0" executable_path test_type "[n_variables]" "[n_tests]"
      echo Test types aviailable: sopp dsopp
      exit
    fi
    test_type=$2
    if (($# > 2)); then
      n_variables=$3
      if(($# > 3)); then
        n_tests=$4
      fi
    fi
  else
    echo executable was not found in the default path, please use:
    echo bash "$0" executable_path test_type "[n_variables]" "[n_tests]"
    exit
  fi
fi

#recap
echo Comparing performance and output of "$test_type" synthesis with "$n_variables" variables over "$n_tests" tests

#execute program and store time of execution
{ time "$executable_path" "$test_type""_time" "$n_variables" "$n_tests" > $program_out 2> execution_errors; } 2>> $test_result_filename"$test_type"
{ time "$executable_path" "$test_type""_e_time" "$n_variables" "$n_tests" > $program_e_out 2> execution_errors_e; } 2>> $test_result_filename"$test_type""_e"

#retrieve time of execution
sec_standard=$(cut -d, -f1 $test_result_filename"$test_type")
msec_standard="10#"$(cut -d, -f2 $test_result_filename"$test_type")
sec_experimental=$(cut -d, -f1 $test_result_filename"$test_type""_e")
msec_experimental="10#"$(cut -d, -f2 $test_result_filename"$test_type""_e")

#print results
echo standard runtime is $((sec_standard * 1000 + msec_standard)) ms
echo experimental runtime is $((sec_experimental * 1000 + msec_experimental)) ms

#retrieve weights of standard
standard_weights=0
count=0
while IFS= read -r line; do
  standard_weights=$((standard_weights + line))
  count=$((count+1))
done < "$program_out"

#retrieve weights of experimental
experimental_weights=0
count_e=0
while IFS= read -r line; do
  experimental_weights=$((experimental_weights + line))
  count_e=$((count_e+1))
done < "$program_e_out"

#print weights
echo average sum of weight in standard synthesis is $((standard_weights / count))
echo average sum of weight in experimental synthesis is $((experimental_weights / count_e))

#remove tmp files
rm $test_result_filename*
rm $program_out
rm $program_e_out
