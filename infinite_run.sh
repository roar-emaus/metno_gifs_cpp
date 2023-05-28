#!/bin/bash

# URL of the file to check
check_url="https://thredds.met.no/thredds/dodsC/metpplatest/met_forecast_1_0km_nordic_latest.nc.dds"

# URL of the file to download
download_url="https://thredds.met.no/thredds/fileServer/metpplatest/met_forecast_1_0km_nordic_latest.nc"

# Path to the local file
local_file="/input/metno_pp.nc"

# Function to handle errors
handle_error() {
    echo "Error on line $1"
    exit 1
}

# Call the error handling function whenever a command fails
trap 'handle_error $LINENO' ERR

# Start infinite loop
while true
do
    # Get the current time
    current_time=$(date)

    # Get the last-modified timestamp from the remote server
    remote_timestamp=$(curl -sI $check_url | grep -i 'Last-Modified' | awk '{ print substr($0, index($0,$2)) }')

    # If curl fails and the remote timestamp is empty
    if [ -z "$remote_timestamp" ]; then
        echo "Failed to fetch remote timestamp. Retrying in 5 minutes..."
        sleep 300
        continue
    fi

    # Convert the remote timestamp to Unix time
    remote_unix_timestamp=$(date -d"$remote_timestamp" +%s)

    # If the local file does not exist or stat fails, set local_unix_timestamp to 0
    if [ ! -f "$local_file" ]; then
        local_unix_timestamp=0
    else
        local_unix_timestamp=$(stat -c %Y "$local_file" 2>/dev/null) || local_unix_timestamp=0
    fi

    # Convert the Unix time to human-readable format
    remote_readable_timestamp=$(date -d @$remote_unix_timestamp)
    local_readable_timestamp=$(date -d @$local_unix_timestamp)

    echo "Current time: $current_time"
    echo "Remote timestamp: $remote_readable_timestamp"
    echo "Local timestamp: $local_readable_timestamp"

    # If the remote file is newer, download it and run the image generation program
    if [ $remote_unix_timestamp -gt $local_unix_timestamp ]; then
        echo "Remote file is newer. Downloading and updating the local file."
        if curl -# -o $local_file $download_url; then
            metno_gif --no_download
        else
            echo "Failed to download file. Retrying in 5 minutes..."
        fi
    fi

    # Wait for 5 minutes
    echo "Sleeping for 5 minutes..."
    sleep 300
done
