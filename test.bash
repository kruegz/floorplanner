#!/bin/bash

for testcase in n10hard n30hard n50hard n100hard n300hard; do
    
    echo "TEST: $testcase"

    ./floorplanner "in/$testcase.blocks"

    if ./testFloorPlan.py "in/$testcase.blocks" "in/$testcase.out" | grep -q "ERROR"; then
        echo "$testcase ERROR!"
    else
        echo "$testcase SUCCESS!"
    fi

done
