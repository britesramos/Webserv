#!/usr/bin/python3
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import os
import sys
import json

UPLOAD_DIR = "www/uploads/"

print("Content-type: text/html\n")

method = os.environ.get("REQUEST_METHOD", "")
filename = ""
if method in ("DELETE", "POST"):
    try:
        content_length = int(os.environ.get("CONTENT_LENGTH", 0))
        body = sys.stdin.read(content_length)
        data = json.loads(body)
        filename = data.get("filename", "").strip()
    except Exception as e:
        print(f"<p style='color:#d87c88;'>Error: Invalid request body. {e}</p>")
        sys.exit(0)
else:
    from urllib.parse import parse_qs
    qs = os.environ.get("QUERY_STRING", "")
    params = parse_qs(qs)
    filename = params.get("filename", [""])[0].strip()

filepath = os.path.join(UPLOAD_DIR, filename)

if not filename:
    print("<p style='color:#d87c88;'>Error: No filename provided.</p>")
elif "/" in filename or "\\" in filename:
    print("<p style='color:#d87c88;'>Error: Invalid filename.</p>")
elif not os.path.exists(filepath):
    print("<p style='color:#d87c88;'>Error: File not found.</p>")
else:
    try:
        os.remove(filepath)
        print(f"<p style='color:#6b4c4c;'>Pattern <b>{filename}</b> deleted successfully.</p>")
    except Exception as e:
        print(f"<p style='color:#d87c88;'>Error: Could not delete file. {e}</p>")