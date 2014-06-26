#!/bin/bash

set -e

SSL=/usr/local/opt/openssl/bin/openssl

echo "Generating key..."
$SSL genrsa -out com.apple.kerberos.kdc.private 1024
$SSL rsa -in com.apple.kerberos.kdc.private -pubout > com.apple.kerberos.kdc.public

echo "Generating certificate..."
$SSL req -config openssl-kdc.cnf -days 7300 -outform pem -out com.apple.kerberos.kdc -new -key com.apple.kerberos.kdc.private -nodes -x509 -sha1

echo "Install certificate and keys... (requires sudo)"
cat com.apple.kerberos.kdc com.apple.kerberos.kdc.private com.apple.kerberos.kdc.public > combined.pem
sudo security import combined.pem -k /Library/Keychains/System.keychain -t agg -f openssl

