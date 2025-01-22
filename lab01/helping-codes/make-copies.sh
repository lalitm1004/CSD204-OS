#!/bin/bash

i=-1
while [ $i -lt 5000 ]; do
    i=$((i + 1))
    cp "./foo.pdf" "./temp-files/foo$i.pdf"
done