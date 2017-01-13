#!/bin/bash

MMDfile=$(sed -E "s/^(.*)\\\\$/\1/g" Camera.d)

for line in "$MMDfile"
do
    echo "$line"
done
