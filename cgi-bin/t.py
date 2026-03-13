#!/usr/bin/env python3
import os
import sys

length_str = os.environ.get("CONTENT_LENGTH")
length = int(length_str) if length_str else 0

body = sys.stdin.read(length)

print("Content-Type: text/plain")
print()

print("Body size:", len(body))
print(body)