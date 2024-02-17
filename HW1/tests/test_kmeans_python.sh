#!/bin/bash

SCRIPT_PATH="kmeans.py"
declare -a inputs=("tests/input_1.txt" "tests/input_2.txt" "tests/input_3.txt")
declare -a outputs=("tests/output_1.txt" "tests/output_2.txt" "tests/output_3.txt")
declare -a ks=(3 7 15)
declare -a max_iters=(600 0 300)

for index in "${!inputs[@]}"; do
    INPUT_FILE="${inputs[$index]}"
    EXPECTED_OUTPUT_FILE="${outputs[$index]}"
    K="${ks[$index]}"
    MAX_ITER="${max_iters[$index]}"
    ACTUAL_OUTPUT_FILE="actual_output_${index}.txt"

    echo "Running test for $INPUT_FILE and $EXPECTED_OUTPUT_FILE with K=$K and max_iter=${MAX_ITER:-'default'}..."

    if [ "${MAX_ITER}" -eq 0 ]; then
        COMMAND="python3 $SCRIPT_PATH $K $INPUT_FILE"
    else
        COMMAND="python3 $SCRIPT_PATH $K $MAX_ITER $INPUT_FILE"
    fi
    
    echo "Executing command: $COMMAND"
    eval $COMMAND >$ACTUAL_OUTPUT_FILE 2>&1

    # Check the exit status of the last command
    if [ $? -ne 0 ]; then
        echo -e "\033[1;31mError executing command: $COMMAND\033[0m"
        echo "Python output:"
        echo -e "\033[35m" # Start purple color
        cat $ACTUAL_OUTPUT_FILE # Display Python output/error
        echo -e "\033[0m" # Reset to default color
        echo -e "\033[1;31m\033[1mTEST FAIL\033[0m"
    else

        echo "Checking for differences..."

        line_number=0
        test_fail=false

        while IFS= read -r line_exp <&3 && IFS= read -r line_act <&4; do
            ((line_number++))
            if [ "$line_exp" != "$line_act" ]; then
                test_fail=true
                echo -e "\033[1;31mLine $line_number: Expected '$line_exp' but got '$line_act'\033[0m"
                break
            fi
        done 3<$EXPECTED_OUTPUT_FILE 4<$ACTUAL_OUTPUT_FILE

        if $test_fail; then
            echo -e "\033[1;31m\033[1mTEST FAIL\033[0m"
        else
            echo -e "\033[1;32m\033[1mTEST PASS\033[0m"
        fi
    fi

    rm $ACTUAL_OUTPUT_FILE
    echo "------------------------------------------------"
done
