#!/usr/bin/env python3
import sys
import os

sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n")
sys.stdout.write("<h1>CGI works</h1>\r\n")
sys.stdout.flush()
while True:
    print("ok")