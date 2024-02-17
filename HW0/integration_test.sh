#!/bin/bash

# Compile the C program
gcc -o converter_program 319127981_328488572_bc.c -lm

# Function to run tests
run_test() {
    local source_base=$1
    local target_base=$2
    local num_to_convert=$3
    local expected_output=$4

    # Run the program and capture the output
    output=$(./converter_program <<EOF
$source_base
$target_base
$num_to_convert
EOF
    )

    # Extract actual output (just the number without text)
    actual_output=$(echo "$output" | awk '/The number in base/ {print $NF}')

    # Check for errors and print them
    if [ -z "$actual_output" ]; then
        echo "Error: Unexpected output!"
    fi

    echo "Actual Output: $actual_output"
    echo "Expected Output: $expected_output"

    # Compare actual and expected output
     if [ "$actual_output" == "$expected_output" ]; then
        echo -e "\033[32mTest Passed!\033[0m"  # Green color for success
    else
        echo -e "\033[31mTest Failed!\033[0m" 
    fi

    echo "-----------------------------------------------------"
}

# Run multiple tests with expected values
run_test 10 2 255 11111111  # Decimal to Binary
run_test 2 10 1101 13       # Binary to Decimal
run_test 8 16 377 ff     # Octal to Hex
run_test 16 10 abc 2748          # Hex to Decimal (Invalid input)
run_test 5 8 123 46              # Invalid source base

# Clean up
rm converter_program
