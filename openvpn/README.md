==== Creating SSL certificates using EasyRSA-3 ========
 
To use OpenVPN is necessary to create self-signed certificates for the client and the server. The certificates should be signed by the same certification authority (CA), whose certificate must also be created. This is done using "openssl". However, a series of scripts called EasyRSA hugely simplify the process:

1) First download EasyRSA scripts somewhere on your computer and put these executable on the PATH:
```
wget https://github.com/OpenVPN/easy-rsa/releases/download/v3.0.8/EasyRSA-3.0.8.tgz
tar xvzf EasyRSA-3.0.8.tgz
export PATH=$PATH:$PWD/EasyRSA-3.0.8
```

2) Somewhere else on your computer build the PKI (Public Key Infrastructure) directory, which will contain all the signed certificates (and also allows to revoke them).

easyrsa init-pki

This will create the ./pki directory which contains the public/private keys and the certification authority.

3) Now create the CA certificate. From the same position type:

```
easyrsa build-ca nopass
[choose Common Name]
```

This will create the CA certificate pki/ca.crt which will be use to sign the certificate of the server and clients. It also create the private key associated to that certificate (pki/private/ca.key) that for security reasons should not be kept on the server.


4) Create and sign a server and client certificate (typing from the same dir):

```
easyrsa gen-req myserver nopass
easyrsa sign-req server myserver

easyrsa gen-req myclient nopass
easyrsa sign-req client myclient
```

The "gen-req" command creates the request file (pki/reqs/myserver.req) that contains the public key of the server, and the private key file (pki/private/myserver.key). The request file is sent to the CA for signature. The request file should be created from a different machine then CA because the server or the client should never see the private key of the CA. However, in this setup everything is done on the same machine for simplicity.

The signed certificates are placed into pki/issued/myserver.crt and pki/issued/myclient.crt



5) The server finally needs a Diffie-Hellman pair. This is computationally expensive and can be done here in advance:
```
easyrsa gen-dh       
```
which creates the file pki/dh.pem after a while





