#!/usr/bin/python3
import cgi
import os
import sys

def main():
    print("Content-Type: text/html\n")
    
    try:
        form = cgi.FieldStorage()
        
        # Check if file was uploaded
        if "image" not in form:
            print("<h1>Error: No file uploaded</h1>")
            return
        
        fileitem = form["image"]
        
        # Test if the file was uploaded
        if not fileitem.filename:
            print("<h1>Error: No file selected</h1>")
            return

        # Get file extension
        _, ext = os.path.splitext(fileitem.filename)
        allowed_extensions = ['.jpg', '.jpeg', '.png', '.pdf']
        
        if ext.lower() not in allowed_extensions:
            print("<h1>Error: Invalid file type</h1>")
            return
            
        # Create upload directory if it doesn't exist
        upload_dir = "/tmp/uploads"
        if not os.path.exists(upload_dir):
            os.makedirs(upload_dir)
            
        # Save the file
        filepath = os.path.join(upload_dir, fileitem.filename)
        with open(filepath, 'wb') as f:
            f.write(fileitem.file.read())
            
        print("<h1>Upload successful!</h1>")
        print(f"<p>File saved as: {fileitem.filename}</p>")
        print('<script>window.location.href="/html/Success.html";</script>')
        
    except Exception as e:
        print(f"<h1>Error occurred</h1><p>{str(e)}</p>")
        sys.stderr.write(f"Upload error: {str(e)}\n")

if __name__ == "__main__":
    main()