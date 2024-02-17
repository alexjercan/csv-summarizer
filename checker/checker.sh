#!/bin/bash

make clean && make

# read from stdin and write to stdout
rm -f result.csv
cat example.csv | ./main mass,size,price > result.csv
diff result.csv expected.csv

if [ $? -eq 0 ]; then
    echo "Test 1 passed"
else
    echo "Test 1 failed"
fi

# read from file and write to stdout
rm -f result.csv
./main mass,size,price --input example.csv > result.csv
diff result.csv expected.csv

if [ $? -eq 0 ]; then
    echo "Test 2 passed"
else
    echo "Test 2 failed"
fi

# read from stdin and write to file
rm -f result.csv
cat example.csv | ./main mass,size,price --output result.csv
diff result.csv expected.csv

if [ $? -eq 0 ]; then
    echo "Test 3 passed"
else
    echo "Test 3 failed"
fi

# read from file and write to file
rm -f result.csv
./main mass,size,price --input example.csv --output result.csv
diff result.csv expected.csv

if [ $? -eq 0 ]; then
    echo "Test 4 passed"
else
    echo "Test 4 failed"
fi

rm -f result.csv
