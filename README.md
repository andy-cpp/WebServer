# WebServer
This is a webserver made purely in c++ without the use of any external libraries.
## Advantages
- SSL Support
- Proxy Support
- PHP Support
- LFI Protection
- Lightweight web server

## Configuration

Example Configuration <br>
configs.json - an array of configurations / virtual hosts
```json
[
  "main.json"
]
```

main.json
```json
{
  "ip": "0.0.0.0",
  "port": 8080,
  "ssl": false,
  "webroot": "../html",
  "default_site": "index.html"
}
```
Proxy example <br>
main.json
```json
{
  "ip": "0.0.0.0",
  "port": 8080,
  "ssl": false,
  "webroot": "../html",
  "default_site": "index.html",
  "proxy": "127.0.0.1:7000"
}
```
SSL Example <br>
main.json
```json
{
  "ip": "0.0.0.0",
  "port": 8080,
  "ssl": true,
  "ssl_certfile": "cert.pem",
  "ssl_keyfile": "key.pem"
  
  "webroot": "../html",
  "default_site": "index.html",
}
```
