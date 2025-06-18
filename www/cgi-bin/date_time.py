#!/usr/bin/python3

import datetime

# print("Content-type: text/html\n")

x = datetime.datetime.now()
print(f'''
<div style='text-align: center; color: #a05270;'>
    <h3 style='margin-bottom: 15px;'>Current Date and Time:</h3>
    <p style='font-size: 1.2em; color: #6b4c4c;'>{x.strftime("%c")}</p>
    <p style='margin-top: 15px; font-size: 0.9em; color: #6b4c4c;'>Time Zone: {x.astimezone().tzname()}</p>
</div>
''')
fhjfshdvsjvhwpsdjskdf