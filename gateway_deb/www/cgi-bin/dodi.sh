#!/bin/sh

###
 # @Description: 
 # @Version: 1.0
 # @Autor: Towel Roll
 # @Date: 2022-03-10 15:16:48
 # @LastEditors: Towel Roll
 # @LastEditTime: 2022-03-11 16:37:53
### 


echo -e "Content-type: text/plain\n"

DI=`gpioget 0 2`

# REQUEST_METHOD="POST"
# receive_string="DO_Status=1"

if [ "$REQUEST_METHOD" = "POST" ]; then
    receive_string=`cat`
    # echo $receive_string
    if [ "$receive_string" = "DO_Status=1" ]; then
        DO_1=`gpioset 0 0=1`
        exit 0
    elif [ "$receive_string" = "DO_Status=0" ]; then
        DO_0=`gpioset 0 0=0`
        exit 0
    fi
elif [ "$REQUEST_METHOD" = "GET" ]; then
    # echo $QUERY_STRING
    if [ "$QUERY_STRING" = "DI_Status" ]; then
        if [ "$DI" = "1" ]; then
            echo 0
        else
            echo 1
        fi
        exit 0
    fi
fi

echo "error"