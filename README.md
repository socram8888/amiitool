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

Protocol
--------
Well, initially I was only going to support decryption, so I published a simple way to decrypt stuff using netcat alone. I wanted then to add encryption, but some people had already coded their own clients, so I had to implement a backwards-compatible protocol, which had to be lightweight (due to server running on a resource-constrained VPS) and platform-independent also. So this is how it works:

 - Decryption:  
   To decrypt, just send 540 bytes representing amiibo™'s encrypted contents, which *must* start with 0x04 (NXP manufacturer code). Server will reply with 540 bytes, which is amiibo™'s data after decryption.

    - Request:  
	    ```c
		struct decrypt_request {
	        uint8_t encrypted[540];
		}
	    ```

    - Reply:  
	    ```c
		struct decrypt_reply {
	        uint8_t decrypted[540];
		}
	    ```

 - Encryption:

    - Request:  
	    ```c
		struct encrypt_request {
			char op = 'E';
			struct auth auth;
			uint8_t decrypted[540];
		}
	    ```

    - Reply:  
	    ```c
		struct encrypt_reply {
			uint8_t encrypted[540];
		}
	    ```

 - Terms of service:  
   Wall of text which can be resumed actually in "use this as long as you want but don't be an asshole". Reply is a variable-length UTF8 blob.

    - Request:  
	    ```c
		struct tos_request {
			char op = 'T';
		}
	    ```

    - Reply: variable-length UTF8 blob.

Authentication structure is as follows:
```c
struct auth {
	char version;
	char userid[16];
	char token[16];
}
```

User ID are first 16 hexadecimal characters from user's API key (in ASCII, not binary), while the remaining 16 characters are the first 16 characters of the hexadecimal representation of the SHA256 hash of current UNIX time divided by 30 followed by "-" and user's API key last 16 characters.

In C, using OpenSSL, with user's key in ASCII:
```c
void to_hex(char * hex, const void * data, size_t size) {
	const uint8_t * bytes = (const uint8_t *) data;
	size_t pos;

	for (pos = 0; pos < size; pos++) {
		hex[pos * 2 + 0] = "0123456789ABCDEF"[bytes[pos] / 16];
		hex[pos * 2 + 1] = "0123456789ABCDEF"[bytes[pos] % 16];
	}
}

void calc_auth(struct auth * auth, const char apikey[32]) {
	// Set version to 1
	auth->version = '1';

	// Copy fixed version (user's ID)
	memcpy(auth->userid, apikey, 16);

	// Prepare SHA256 input
	char tmp[32];
	uint32_t now = time(NULL) / 30;
	int len = sprintf(tmp, "%d-%s", now, apikey + 16);

	// Hash it
	uint8_t hash[32];
	EVP_Digest(tmp, now, hash, 32, EVP_sha256(), NULL);

	// Now convert to hex first 8 bytes
	to_hex(auth->token, hash, 8);
}
```
