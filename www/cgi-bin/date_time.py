#!/usr/bin/python3

import cgi
import datetime

# print("Content-type: text/html\n\n")
print("<html><body style='text-align:center;'>")

print("<h1 style='color: pink;'>This is the date and time now</h1>")

x = datetime.datetime.now()
print("<strong>", x.strftime("%c"), "</strong>")
print("<br>")
print("<br>")
print("<a href=\"../www/html/.html\">Return home</a>")

print("</body></html>")