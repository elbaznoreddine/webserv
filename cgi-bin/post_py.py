#!/usr/bin/env python3
import os
import sys

length = int(os.environ.get("CONTENT_LENGTH") or 0)
body = sys.stdin.read(length) if length > 0 else ""

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<h1>POST works</h1>")
print(f"<p>CONTENT_LENGTH: {length}</p>")
print(f"<p>BODY: {body}</p>")
print("</body></html>")