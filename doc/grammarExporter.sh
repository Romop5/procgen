#!/bin/bash
# Roman Dobias <xdobia11<AT>stud.fit.vutbr.cz>

# Extract Bison grammar from cpp file
EXTRACTED_GRAMMAR=`cat ../src/procgen/parser/parser.y | csplit - %\%\%% && cat xx00 | egrep "(\||:)"`


###################
# Remove C++ code
###################

# Remove { }
EXTRACTED_GRAMMAR=`echo "$EXTRACTED_GRAMMAR" | sed 's/{.*;.*}//g'`

# Remove //
EXTRACTED_GRAMMAR=`echo "$EXTRACTED_GRAMMAR" | sed 's/\/\/.*//g'`


###################
# Transform from Bison to BNF
###################

# %empty to ""
EXTRACTED_GRAMMAR=`echo "$EXTRACTED_GRAMMAR" | sed 's/%empty/""/g'`
# Transform xAy to x-ay
EXTRACTED_GRAMMAR=`echo "$EXTRACTED_GRAMMAR" | sed 's/\([a-z]\+\)\([A-Z]\)/\1-\L\2/g'`
# Transform non-terminals to <non-terminal> form
EXTRACTED_GRAMMAR=`echo "$EXTRACTED_GRAMMAR" | sed 's/\([a-z\-]\+\)/<\U\1>/g'`

# Transform : to ::=
EXTRACTED_GRAMMAR=`echo "$EXTRACTED_GRAMMAR" | sed 's/:/::=/g'`


echo "Final grammar"
echo "$EXTRACTED_GRAMMAR"


