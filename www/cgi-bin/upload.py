import os
import cgi
import sys

upload_dir = "www/uploads"
try:
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)
except Exception as e:
    print(f"Error creating directory: {str(e)}")

form = cgi.FieldStorage()
if "image" not in form:
    print("No file uploaded from HTML form.")
    sys.exit(0)
fileitem = form["image"]
if not fileitem.filename:
    print("No file selected in HTML form.")
    sys.exit(0)
filename = os.path.basename(fileitem.filename)
allowed_extensions = {'.jpg', '.jpeg', '.png', '.gif'}
_, ext = os.path.splitext(filename)
if ext.lower() not in allowed_extensions:
    print("Error: Only image files (.jpg, .jpeg, .png, .gif) are allowed.")
    sys.exit(0)


filepath = os.path.join(upload_dir, filename)
try:
    with open(filepath, 'wb') as f:
        f.write(fileitem.file.read())
    print(f"File uploaded from HTML and saved as: {filepath}")
except Exception as e:
    print(f"Error saving uploaded file: {str(e)}")