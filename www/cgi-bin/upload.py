#!/usr/bin/python3

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import os
import cgi
import sys

print("Content-type: text/html\n")

upload_dir = "www/uploads"
try:
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)
except Exception as e:
    print(f"<p style='color:#d87c88;'>Error creating directory: {str(e)}</p>")
    sys.exit(0)

form = cgi.FieldStorage()
if "image" not in form:
    print("<p style='color:#d87c88;'>No file uploaded from HTML form.</p>")
    sys.exit(0)
fileitem = form["image"]
if not fileitem.filename:
    print("<p style='color:#d87c88;'>No file selected in HTML form.</p>")
    sys.exit(0)
filename = os.path.basename(fileitem.filename)
allowed_extensions = {'.jpg', '.jpeg', '.png', '.gif'}
_, ext = os.path.splitext(filename)
if ext.lower() not in allowed_extensions:
    print("<p style='color:#d87c88;'>Error: Only image files (.jpg, .jpeg, .png, .gif) are allowed.</p>")
    sys.exit(0)


filepath = os.path.join(upload_dir, filename)
try:
    with open(filepath, 'wb') as f:
        f.write(fileitem.file.read())
    print(f"<p style='color:#6b4c4c;'>File uploaded and saved as: {filename}</p>")
except Exception as e:
    print(f"<p style='color:#d87c88;'>Error saving uploaded file: {str(e)}</p>")