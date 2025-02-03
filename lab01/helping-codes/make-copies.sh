#!/bin/bash

i=-1
while [ $i -lt 5000 ]; do
    i=$((i + 1))
    cp "./foo.pdf" "./tmp/foo$i.pdf"
done
