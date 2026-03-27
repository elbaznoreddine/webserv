#!/usr/bin/env python3
# test.py

import os
import urllib.parse

# Get raw query string
query_string = os.environ.get('QUERY_STRING', '')

# Parse key-value pairs
params = urllib.parse.parse_qs(query_string)

# Access values (parse_qs returns lists)
name = params.get('name', ['Guest'])[0]
age  = params.get('age', ['unknown'])[0]

# Output headers and content
print("Content-Type: text/plain\n")
print(f"Name: {name}")
print(f"Age: {age}")