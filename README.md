amiitool
========
amiibo™ encryption/decryption tool

Usage
=====
To use this application you have two choices:

Standalone
----------
Using amiitool standalone means you need to compile amiitool yourself targeting your own operating system, and using the amiibo™ cryptographic keys that you must obtain by your own means.

Please **do not ask me for the keys**, or ask me how to obtain them. amiitool is not a toy but a reverse-engineering tool, and **if you have no idea on how to obtain these keys, certainly this just isn't for you**.

Networked
---------
If you can't or feel like getting these keys by your own means, you can still help reverse-engineering amiibo™ data by using a free service I've put available for everyone.

This method uses a dummy bash client running on your computer that sends dumps using a simple and lightweight protocol to a remote amiitool instance running on my own VPS for processing.

Given this is publicily available, and to prevent cheating or counterfeiting, this **requires an API key to encrypt, but not to decrypt**.

Standalone usage
================
**amiitool expects a binary dump. It will not work with XMLs or hexadecimal text files**. Aside from this, amiitool is very easy to use and has a very simple syntax.

First, you have to specify an operation, either ```-e``` (encrypt and sign) or ```-d``` (decrypt and check).

You need also to specify a file using ```-k [keys]``` switch, indicating which file contains the cryptographic master keys. **For retail amiibo™, use retail unfixed key set**.

Optionally, you may also specify input and output files using ```-i [input]``` and ```-o [output]```. If input or output are unspecified, amiitool will default to stdin and stdout, respectively. This lets you pipe amiitool inputs and outputs with standard shell tools such as xxd.

When decrypting, by default amiitool will be in strict mode, and will abort and raise an error if the cryptographic signature embedded in the encrypted dump is not valid. If you want to disable checking, use ```-l``` switch to put amiitool in lenient mode.

Examples
--------

- Decryption "mario.bin" and displaying hex to terminal:
   > amiitool -d -k retail_unfixed.bin -i "mario.bin" | xxd

- Encryption "modified.bin" to "signed.bin":
   > amiitool -e -k retail_unfixed.bin -i "modified.bin" -o "signed.bin"

Networked usage
===============
amiicli.sh is a client you may use on your computer, with [Cygwin](https://cygwin.com) under Windows or any Bash-compatible shell under Linux, that initializes a connection to the remote amiitool server, executes authorization sequence if needed, sends command and data (if any), and waits for either a reply or an error code.

Obviously due to the server-client architecture, you will need an internet connection, and be sure that your NAT and firewalls don't restrict non-standard protocols or connections to port 22403, which is likely on hotspots.

To use amiicli.sh, as with amiitool itself, you need to specify first a request code, which may be either "e" (encrypt), "d" (decrypt) or "t" (terms of service).

After the request code, there are two optional parameters which are *always* input file and output file, even for commands such as ToS that expect no input. If these are not specified, amiicli.sh will use stdin and stdout as in standalone mode.

For encryption, to prevent cheating counterfeiting, encryption requires an API key. Those keys are manually generated on a case-by-case basis, and exclusively to help reverse-engineering amiibo™'s contents. If you need the key, contact me at <socram@protonmail.ch>.

Examples
--------

- Decryption "mario.bin" and displaying hex to terminal:
   > ./amiicli.sh d mario.bin | xxd

- Encryption "modified.bin" to "signed.bin":
   > ./amiicli.sh e modified.bin signed.bin
