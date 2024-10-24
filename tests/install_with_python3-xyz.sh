#!/bin/bash

while IFS= read -r package; do
    apt install python3-"$package_name"
done < requirements.txt
