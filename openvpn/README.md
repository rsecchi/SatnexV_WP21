==== Creating SSL certificates using EasyRSA-3 ========
 
To use OpenVPN is necessary to create self-signed certificates for the client and the server. The certificates should be signed by the same certification authority (CA), whose certificate must also be created. This is done using "openssl". However, a series of scripts called EasyRSA hugely simplify the process:

1) First download EasyRSA scripts somewhere on your computer and put these executable on the PATH:
```
wget https://github.com/OpenVPN/easy-rsa/releases/download/v3.0.8/EasyRSA-3.0.8.tgz
tar xvzf EasyRSA-3.0.8.tgz
export PATH=$PATH:$PWD/EasyRSA-3.0.8
```

2) Somewhere else on your computer build the PKI (Public Key Infrastructure) directory, which will contain all the signed certificates (and also allows to revoke them).
```
easyrsa init-pki
```
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
which creates the file pki/dh.pem after a while.





==== Creating OpenVPN server and client configuration files ========

The following configuration code should be inserted in server.conf

```
; configuration file server.conf

; The interface is TAP (a L2 encapsulation) and will appear 
; in the server as tap0 interface
dev tap0

; The interface is persistent. This means that it exists 
; even if the server does not run and should be created in advance
; using the commmand "openvpn --mkdev --
persist-tun


; This sends a keepalive every 10 seconds for 2 minutes before
; declaring the client dead
keepalive 10 120


; The server is use as a brige to the clients
; The server address is 10.1.1.5/24 client are between .10 .20
server-bridge 10.1.1.5 255.255.255.0 10.1.1.10 10.1.1.20

```

Then, embed all the certificates and the DH pair in the configuration file (or provide pointers):

```
echo '<ca>' >> server.conf
cat pki/ca.crt >> server.conf
echo '</ca>' >> server.conf

echo '<cert>' >> server.conf
cat pki/issued/myserver.crt >> server.conf
echo '</cert>' >> server.conf

echo '<key>' >> server.conf
cat pki/private/myserver.key >> server.conf
echo '</key>' >> server.conf

echo '<dh>' >> server.conf
cat pki/dh.pem >> server.conf
echo '</dh>' >> server.conf

```

The file server.conf is ready to be checked:
```
openvpn --config server.conf
```


