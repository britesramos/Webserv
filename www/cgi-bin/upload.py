#!/usr/bin/python3

import cgi
import os

# print("Content-type: text/html\n\n")

form = cgi.FieldStorage()
fileitem = form['image']

directory_name = "/tmp/uploads/"

# Create the directory
try:
    os.mkdir(directory_name)
    print(f"Directory '{directory_name}' created successfully.")
except FileExistsError:
    print(f"Directory '{directory_name}' already exists.")
except PermissionError:
    print(f"Permission denied: Unable to create '{directory_name}'.")
except Exception as e:
    print(f"An error occurred: {e}")



#     print(f"<h1>Upload successful</h1><p>Saved as {filename}</p>")


# print("</body></html>")