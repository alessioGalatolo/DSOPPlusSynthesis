#!/bin/bash

#this script is used to compare the time of sopp and dsopp synthesis

executable_path="./cmake-build-debug/DSOPP_synthesis" #default executable path
test_result_filename="test_tmp_results"
TIMEFORMAT="%5R" #for precision in measuring runtime
n_tests=100 #default value
n_variables=6 #default value
program_out="synthesis_weights.out" #file for collecting the weights of the standard forms
program_e_out="synthesis_e_weights.out" #file for collecting the weights of the experimental forms
INT_MAX=2147483647

#check arguments
if (($# == 0)); then #not enough arguments
  echo Usage: bash "$0" test_type "[n_variables]" "[n_tests]"
  echo Test types aviailable: sopp dsopp
  exit
else
  test_type=$1
  if (($# > 1)); then #was given a value for the number of variables to use
    n_variables=$2
    if(($# > 2)); then #was given a value for the number of tests to do
      n_tests=$3
    fi
  fi
fi

#test executable existance
if ! [ -f $executable_path ]; then
  if [ $# -gt 0 ] && [ -f "$1" ]; then
    #if invalid default path, check for a given one
    executable_path=$1
    if ! (($# > 1)); then
      #valid executable path was provided but not enough arguments
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
    #no valid executable path was provided
    echo executable was not found in the default path, please use:
    echo bash "$0" executable_path test_type "[n_variables]" "[n_tests]"
    exit
  fi
fi

#recap
echo Comparing performance and output of "$test_type" synthesis with "$n_variables" variables over "$n_tests" tests

#execute program and store time of execution, redirect errors to a different file
{ time "$executable_path" "$test_type""_time" "$n_variables" "$n_tests" > $program_out 2> execution_errors; }\
 2>> $test_result_filename"$test_type"
{ time "$executable_path" "$test_type""_e_time" "$n_variables" "$n_tests" > $program_e_out 2> execution_errors_e; }\
 2>> $test_result_filename"$test_type""_e"

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
standard_max=0
standard_min=$INT_MAX
count=0
while IFS= read -r line; do
  standard_weights=$((standard_weights + line))
  if (( line > standard_max )); then
    standard_max=$line
  fi
  if (( line < standard_min )); then
    standard_min=$line
  fi
  count=$((count+1))
done < "$program_out"

#print weights
standard_average=$(bc <<< "scale=2; $standard_weights / $count")
echo average sum of weights in standard synthesis is "$standard_average"
echo max weight in standard synthesis is "$standard_max" while min is "$standard_min"

#retrieve weights of experimental
experimental_weights=0
experimental_max=0
experimental_min=$INT_MAX
count_e=0
while IFS= read -r line; do
  experimental_weights=$((experimental_weights + line))
  if (( line > experimental_max )); then
    experimental_max=$line
  fi
  if (( line < experimental_min )); then
    experimental_min=$line
  fi
  count_e=$((count_e+1))
done < "$program_e_out"

#print weights
experimental_average=$(bc <<< "scale=2; $experimental_weights / $count_e")
echo average sum of weights in experimental synthesis is "$experimental_average"
echo max weight in experimental synthesis is "$experimental_max" while min is "$experimental_min"

#remove tmp files
rm $test_result_filename*
rm $program_out
rm $program_e_out

#remove files conatining exection errors if empty
if [ ! -s ./execution_errors ] ; then
  rm execution_errors
fi
if [ ! -s execution_errors_e ] ; then
  rm execution_errors_e
fi

