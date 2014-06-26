#!/bin/bash
#
# This script requires root and might break things! Make sure you
# read and understand what it does, if you choose to use it you
# do so at your own risk.
#
# This script will:
# * compile registerKDCCertificate binary utility
# * remove existing kerberos data
# * generate and install new kdc certificate (requires sudo)
# * run registerKDCCertificate utility to register new certificate
# * run Apple's configureLocalKDC to reconfigure kerberos
# * clean up created certificates and keys

# make sure you have removed `com.apple.kerberos.kdc`
# cert and keys before running this script.

set -ex

make registerKDCCertificate
sudo rm -rf /etc/krb5.keytab /var/db/krb5kdc
./generate-and-install-kdc-cert.sh
sudo ./registerKDCCertificate
sudo /usr/libexec/configureLocalKDC
rm -f com.apple.kerberos.kdc* combined.pem

