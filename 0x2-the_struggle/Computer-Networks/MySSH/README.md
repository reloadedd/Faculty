# MySSH
```
                        88b           d88                ad88888ba    ad88888ba   88        88
                        888b         d888               d8"     "8b  d8"     "8b  88        88
                        88`8b       d8'88               Y8,          Y8,          88        88
                        88 `8b     d8' 88  8b       d8  `Y8aaaaa,    `Y8aaaaa,    88aaaaaaaa88
                        88  `8b   d8'  88  `8b     d8'    `"""""8b,    `"""""8b,  88""""""""88
                        88   `8b d8'   88   `8b   d8'           `8b          `8b  88        88
                        88    `888'    88    `8b,d8'    Y8a     a8P  Y8a     a8P  88        88
                        88     `8'     88      Y88'      "Y88888P"    "Y88888P"   88        88
                                               d8'                                            
                                              d8'                                             
                                        ＭＡＤＥ ＷＩＴＨ ❤️ ＢＹ ＲＯＳＣＡ ＩＯＮＵＴ
```

*Extracted from [documentation](https://github.com/reloadedd/Faculty/tree/master/0x2-the_struggle/Computer-Networks/MySSH/documentation.pdf):*

> **MySSH** is a simple implementation of the SSH program and has the ability to handle pseudo-terminals over network and encrypt its traffic. It also has an 
> optional feature to use an *One-Time-Password (OTP)* for connecting to the server. It is composed of two components: the client and the server. The server is 
> concurrent and accepts multiple remote clients for which it allocates a pseudoterminal and encrypts their communications.

## Features
- Encrypted communication between the server and the client(s) using OpenSSL
- *Optional* One-Time-Password (OTP) which will be generated for each client which accepts the server's certificate
- The server provides for each client access to a *pseudo-terminal*, which allows the client(s) to use the shell to its full capability (line discipline, TAB 
autocompletion)

## Generate the keys
- The default names for the certificate & private key can be found in the `include/config.h` header file:
```c
/* The certificate file needed by OpenSSL in the server */
#define CERTIFICATE        "cert.pem"
#define CERTIFICATE_KEY    "key.pem"
```
- For security reasons I won't publish my private key and the certificate file in this repo (even though they are self-signed)
- You can generate your own by using this command from [Support Microfocus](https://support.microfocus.com/kb/doc.php?id=7013103):
> ```shell
> openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem
> ```

## Pro tip
- **MySSH** assumes the default terminal size (80x24), but this could be changed manually by the user:
```shell
# Go to another terminal and type (without $)
$ stty -a
# Now, take a note of ’rows’ and ’cols’ values
# Then, go back to the client and type the following,
# replacing the <...> fields accordingly:
$ stty rows <rows from stty> cols <cols from stty>
```