amiicli.sh Network Protocol
===========================

Well, initially I was only going to support decryption, so I published a simple way to decrypt stuff using netcat alone. I wanted then to add encryption, but some people had already coded their own clients, so I had to implement a backwards-compatible protocol, which had to be lightweight (due to server running on a resource-constrained VPS) and platform-independent also. So this is how it works:

Decryption
----------

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

Encryption
----------

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

Terms of service
----------------

Wall of text which can be resumed actually in "use this as long as you want but don't be an asshole". Reply is a variable-length UTF8 blob.

 - Request:  
   ```c
   struct tos_request {
       char op = 'T';
   }
   ```

 - Reply: variable-length UTF8 blob.

Authentication
--------------

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

For example, with API key `762D336215AFBA211A0372B1C8E0A1CF` at UNIX time 1443433695 (09:48:15 28/Sep/2015 UTC), auth would be `1762D336215AFBA21B3676B8CBF18BAC5`.

