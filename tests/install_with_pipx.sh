#!/bin/bash
while IFS= read -r package; do
    package_name=$(echo "$package" | cut -d'=' -f1)
    pipx install "$package_name"
    pipx runpip "$package_name" install "$package"
done < requirements.txt
