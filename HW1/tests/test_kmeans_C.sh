#!/bin/bash

# Path to the C source code
PROGRAM_PATH="./main.c"
# Name of the executable to create
EXECUTABLE="kmeans.out"

# Compile the C program
gcc -ansi -Wall -Wextra -Werror -pedantic-errors -o $EXECUTABLE $PROGRAM_PATH 
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

declare -a inputs=("input_1.txt" "input_2.txt" "input_3.txt")
declare -a outputs=("output_1.txt" "output_2.txt" "output_3.txt")
declare -a ks=(3 7 15)
declare -a ns=(800 430 5000)
declare -a ds=(3 11 5)
declare -a max_iters=(600 0 300)

for index in "${!inputs[@]}"; do
    INPUT_FILE="./tests/${inputs[$index]}"
    EXPECTED_OUTPUT_FILE="./tests/${outputs[$index]}"
    K="${ks[$index]}"
    N="${ns[$index]}"
    D="${ds[$index]}"
    MAX_ITER="${max_iters[$index]}"
    ACTUAL_OUTPUT_FILE="actual_output_${index}.txt"

    echo "Running test for $INPUT_FILE and $EXPECTED_OUTPUT_FILE with K=$K and max_iter=${MAX_ITER:-'default'}..."

   if [ ! -f "$EXPECTED_OUTPUT_FILE" ]; then
        echo -e "\033[1;31mExpected output file $EXPECTED_OUTPUT_FILE does not exist.\033[0m"
        echo -e "\033[1;31m\033[1mTEST FAIL\033[0m"
        continue # Skip to the next iteration of the loop
    fi

    if [ "${MAX_ITER}" -eq 0 ]; then
        COMMAND="./$EXECUTABLE $K $N $D < $INPUT_FILE"
    else
        COMMAND="./$EXECUTABLE $K $N $D $MAX_ITER < $INPUT_FILE"
    fi

    echo "Executing command: $COMMAND"
    eval $COMMAND > $ACTUAL_OUTPUT_FILE
    EXIT_STATUS=$?

    if [ $EXIT_STATUS -ne 0 ]; then
        echo -e "\033[1;31mRuntime error (exit status: $EXIT_STATUS).\033[0m"
        echo -e "\033[1;31m\033[1mTEST FAIL\033[0m"
        rm $ACTUAL_OUTPUT_FILE
        continue # Skip to the next iteration of the loop
    fi

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

    rm $ACTUAL_OUTPUT_FILE
    echo "------------------------------------------------"
done

# Optional: Remove the executable after the tests are done
rm $EXECUTABLE
