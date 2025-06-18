#!/usr/bin/python3
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import os
import sys
import json

UPLOAD_DIR = "www/uploads/"

def print_response(msg):
    # print("Content-type: text/html\r\n")
    print(msg)

method = os.environ.get("REQUEST_METHOD", "GET")

if method == "DELETE":
    try:
        content_length = int(os.environ.get("CONTENT_LENGTH", 0))
        body = sys.stdin.read(content_length)
        data = json.loads(body)
        filename = data.get("filename", "").strip()
    except Exception as e:
        print_response(f"Error: Invalid request body. {e}")
        sys.exit(0)
else:
    import cgi
    form = cgi.FieldStorage()
    filename = form.getfirst("filename", "").strip()

if not filename:
    print_response("Error: No filename provided.")
else:
    if "/" in filename or "\\" in filename:
        print_response("Error: Invalid filename.")
    else:
        filepath = os.path.join(UPLOAD_DIR, filename)
        if os.path.exists(filepath):
            try:
                os.remove(filepath)
                print_response(f"Pattern <b>{filename}</b> deleted successfully.")
            except Exception as e:
                print_response(f"Error: Could not delete file. {e}")
        else:
            print_response("Error: File not found.")