usa8 Desktop (Telegram Desktop 7.2.9, API layer 229)
=====================================================

Run usa8.exe. It keeps its data in a "tdata" folder next to the exe
(or in %APPDATA%\usa8 if that folder is not writable), separate from the
official Telegram Desktop.

Server address
--------------
usa8.exe reads server.json from its own folder at start-up:

  {
    "ip": "192.168.1.108",
    "port": 42000
  }

Edit it and restart usa8.exe - no rebuild needed. All DCs (1-5) go to
this one address. Optional keys:

  "public_key_file": "pubkey.asc"   server RSA key (PEM), path relative to the exe
  "public_key": "-----BEGIN RSA PUBLIC KEY-----\n...\n-----END RSA PUBLIC KEY-----"

Without server.json the address baked in at build time is used.
The built-in RSA key has fingerprint 0xe57178736b089355.

Note: an existing tdata keeps its authorization keys. If you point usa8 at a
different server (or the server was reset), delete tdata and log in again.
