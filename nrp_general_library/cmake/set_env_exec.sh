#!/bin/bash

i=0
for (( i=1; i<="$#"; i++ ))
do
	var=${!i}
	if [[ $var == *=* ]]
	then
		eval "export $var"
	else
		break
	fi
done

exec ${@:$i:$#}

