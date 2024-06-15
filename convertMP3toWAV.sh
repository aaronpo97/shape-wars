#!/bin/bash

if ! [ -x "$(command -v ffmpeg)" ]; then
    echo 'Error: ffmpeg is not installed.' >&2
    exit 1
fi

read -p "Enter the input file path: " input_file
read -p "Enter the output file path: " output_file

ffmpeg -i "$input_file" -acodec pcm_s16le -ac 1 -ar 16000 "$output_file"
