#!/bin/sh
if [ "$XPN_PATH" = "" ]; then
locationOfScript=$(dirname "$(readlink -e "$0")")
# Something like ~\expertn50\built/bin
BuildPath=${locationOfScript%\*}
# Something like ~\expertn50\built
XPN_PATH=${BuildPath%\*}
# Something like ~\expertn50
else
BuildPath=$XPN_PATH\built
fi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BuildPath\bin:$BuildPath\lib:$BuildPath\models
export PATH=$PATH:$BuildPath\bin:D:\expertn50\expertn_tools
export PYTHONPATH=$PYTHONPATH:D:\expertn50\expertn_tools



