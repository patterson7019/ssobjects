Build: 
    $ ant
Run simple telnet server (Ctrl-C to exit server, runs on port 4002):
    $ ant run-telnetserver
Run all tests (must have telnetserver running from previous step):
    $ ant test

Telnet server will accept multiple connections. Any text typed into one
client will be echoed to all other clients.