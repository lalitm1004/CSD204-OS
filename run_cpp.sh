#!/bin/bash

if [ -z "$CF" ]; then
  echo "error > CF environment variable not found"
  exit 1
fi

if [ ! -f "$CF" ]; then
  echo "error > '$CF' does not exist"
  exit 1
fi

OUTPUT="temp_output"

g++ "$CF" -o "$OUTPUT"
if [ $? -ne 0 ]; then
  echo "compilation failed."
  exit 1
fi

./"$OUTPUT"

EXIT_CODE=$?

rm -f "$OUTPUT"

exit $EXIT_CODE
