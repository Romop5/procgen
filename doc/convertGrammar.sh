#!/bin/bash

# 2018 Roman Dobias
# Purpose:
#       This script converts grammar written in BNF form
#       to Bison-compatible form

NEW=`cat GRAMATIKA`
OLD="a"


DIFF=`diff <(echo "$OLD") <(echo "$NEW")`

until [ "$DIFF" = "" ]; do 

    OLD=$NEW
    NEW=`echo "$NEW" \
        | sed 's/-\(.\)/\u\1/       #convert <sth-sth> to <sthSth>'\
        | sed 's/::=/:/             #::= to :'\
        | sed 's/\"\"/%empty/       #replace double doublequotes with %empty (empty symbol string)'\
        | sed 's/<\([^>]*\)>/\1/    #convert <sth> to sth>'`

    DIFF=`diff <(echo "$OLD") <(echo "$NEW")`
done

echo "FINALY"
echo "$NEW"
