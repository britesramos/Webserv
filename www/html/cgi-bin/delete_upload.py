#!/usr/bin/python3

import os

print("Content-type:text/html\r\n\r\n)

if os.path.exists("demofile.txt"):
  os.remove("demofile.txt")
else:
  print("<h1>No file uploaded to be delete</h1>")

print("<br>")
print("<br>")
print("<a href=\"../home_page.html\">Return home</a>")

print("</body></html>")