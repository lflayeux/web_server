#!/usr/bin/env python3
import os
import sys

# Un CGI doit TOUJOURS commencer par des headers HTTP
print("Content-Type: text/html")
print()  # Ligne vide obligatoire entre headers et body

# Ensuite le contenu HTML
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>CGI fonctionne !</h1>")
print("<p>Méthode: {}</p>".format(os.environ.get('REQUEST_METHOD', 'N/A')))
print("<p>Path: {}</p>".format(os.environ.get('PATH_INFO', 'N/A')))
print("<p>Query: {}</p>".format(os.environ.get('QUERY_STRING', 'N/A')))

# Afficher toutes les variables d'environnement
print("<h2>Variables d'environnement :</h2>")
print("<ul>")
for key, value in sorted(os.environ.items()):
    if key.startswith(('REQUEST_', 'HTTP_', 'SERVER_', 'GATEWAY_', 'PATH_', 'QUERY_', 'CONTENT_')):
        print("<li><b>{}</b>: {}</li>".format(key, value))
print("</ul>")

print("</body>")
print("</html>")