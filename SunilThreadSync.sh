#!/bin/bash

# Function to calculate the transfer speed in KB/s
calculate_speed_kbps() {
    local size=$1
    local duration=$2
    if [ $duration -gt 0 ] && [ $size -gt 0 ]; then
        speed=$(echo "scale=2; $size / $duration / 1024" | bc)
        echo "$speed KB/s"
    else
        echo "N/A"
    fi
}

# Function to extract the total transferred size in bytes
extract_size() {
    local output="$1"
    echo "$output" | grep -oP '(?<=total size is )\d+'
}

# File to append the results
csv_file="Incremental_data_5_gb.csv"

# Check if the CSV file exists, if not create it and add headers
if [ ! -f "$csv_file" ]; then
    echo "Timestamp,rsync Duration (s),ThreadRsync Duration (s),Time Difference (s) (rsync - ThreadRsync)" > "$csv_file"
fi

# Measure the time and data size for the original rsync command
start_time=$(date +%s)
original_output=$(rsync -avzP /home/ubuntu/Documents/Sunil/SOURCE/ ashish@192.168.220.135:/home/ashish/Documents/Sunil/Destination/ 2>&1)
end_time=$(date +%s)
original_duration=$((end_time - start_time))
original_size=$(extract_size "$original_output")

# Measure the time and data size for the custom rsync command
start_time=$(date +%s)
custom_output=$(./caller /home/ubuntu/Documents/Sunil/SOURCE/ ashish@192.168.220.135:/home/ashish/Documents/Sunil/Destination1/ 2>&1)
end_time=$(date +%s)
custom_duration=$((end_time - start_time))
custom_size=$(extract_size "$custom_output")

# Calculate speeds
original_speed=$(calculate_speed_kbps $original_size $original_duration)
custom_speed=$(calculate_speed_kbps $custom_size $custom_duration)

# Calculate time difference
time_difference=$((original_duration - custom_duration))

# Display the results
echo "Original rsync command duration: $original_duration seconds"
#echo "Original rsync command speed: $original_speed"
echo "Custom rsync command duration: $custom_duration seconds"
#echo "Custom rsync command speed: $custom_speed"
echo "Time difference rsync and GitRsync : $time_difference seconds"

# Get the current timestamp
timestamp=$(date +"%Y-%m-%d %H:%M:%S")

# Append the results to the CSV file
echo "$timestamp,$original_duration,$custom_duration,$time_difference" >> "$csv_file"

# Remove transferred data at both destinations
rm -rf ashish@192.168.220.135:/home/ashish/Documents/Sunil/Destination/*
rm -rf ashish@192.168.220.135:/home/ashish/Documents/Sunil/Destination1/*

