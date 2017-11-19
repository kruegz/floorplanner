#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

for testcase in n10hard n30hard n50hard n100hard n200hard n300hard n10soft n30soft n50soft n100soft n200soft n300soft; do
    
    echo "TEST: $testcase"
    
    utime="$( TIMEFORMAT='%lU';time ( ./parser in/$testcase.blocks > percent.out  ) 2>&1 1>/dev/null )"

    percent="$(cat percent.out)"
    

    if ./testFloorPlan.py "in/$testcase.blocks" "in/$testcase.out" | grep -q "ERROR"; then
        echo -e "$RED$testcase ERROR!$NC"
    else
        echo -e "$GREEN$testcase SUCCESS!$NC time: $utime percent: $percent"
    fi

done
