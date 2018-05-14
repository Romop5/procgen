#/ bin / bash

HEADER_FILES=`find procgen | grep "\.h"`

#echo "$HEADER_FILES"

for FILE in ${HEADER_FILES};
do

    FILE_RAW =`echo $FILE | sed 's/.*\/\([^\/]*\)$/\1/'`
#echo "$FILE vs $FILE_RAW"
        COMMAND
        = "sed 's/\"$FILE_RAW\"/<$FILE>/'"
#echo $COMMAND

        FILE
        =`echo $FILE | sed 's/\//\\\\\//g'`
#echo $FILE

        COMMAND
        = "find ../test/ | xargs -n1 sed -i 's/\"$FILE_RAW\"/<$FILE>/'" eval $COMMAND

#replace
#find procgen | xargs sed `echo s /\"$FILE_RAW\"/<$FILE>/` -i
            done;
