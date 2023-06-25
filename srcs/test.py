#!/usr/bin/env python3

import cgi

# Get the query string parameters
form = cgi.FieldStorage()

# Get the value of the "name" parameter
name = form.getvalue('name', 'Anonymous')

# Generate the response
response_body = f"Hello, {name}! This is a GET request."

# Set the content-type header
print("Content-Type: text/html")
print()

# Print the response body
print(response_body)