#!/usr/bin/python3
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import os
import cgi

# print("Content-type: text/html\r\n")

form = cgi.FieldStorage()
filename = form.getfirst("filename", "").strip()

# Adjust this path to your upload directory if needed
UPLOAD_DIR = "www/uploads/"

if not filename:
    print("Error: No filename provided.")
else:
    # Prevent directory traversal
    if "/" in filename or "\\" in filename:
        print("Error: Invalid filename.")
    else:
        filepath = os.path.join(UPLOAD_DIR, filename)
        if os.path.exists(filepath):
            try:
                os.remove(filepath)
                print(f"Pattern <b>{filename}</b> deleted successfully.")
            except Exception as e:
                print(f"Error: Could not delete file. {e}")
        else:
            print("Error: File not found.")