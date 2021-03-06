# README

Sometimes your KDC database gets out of sync with your System keychain. This causes OS X get angry when you ask it to authenticate using an Apple ID.

To verify the problem, compare the System keychain certificate `sha1` fingerprint for `com.apple.kerberos.kdc` with your kadmin priciples and the system keytab. Example:

Certificate sha1: `ca3a730aa2a34d8b35e39986b4b17abaaccb2c6f`

```
$ sudo kadmin -r local -l                                                                                        
kadmin> list *
Guest@LKDC:SHA1.ca3a730aa2a34d8b35e39986b4b17abaaccb2c6f
me@LKDC:SHA1.ca3a730aa2a34d8b35e39986b4b17abaaccb2c6f
```

```
sudo ktutil -k /etc/krb5.keytab list
...SHA1.d5b45827ab5068a38fd95e65249fea93127f220e@LKDC:SHA1.d5b45827ab5068a38fd95e65249fea93127f220e
```

If these don't match you can destroy and recreate your KDC certificates and database.

## Recreating your local KDC

 1. Remove the existing "com.apple.kerberos.kdc" certificate and key pair (three items) from the System keychain (optional).
 2. Remove the existing KDC database:
    `sudo rm -rf /var/db/krb5kdc`
 3. Regenerate the KDC database:
    `sudo /usr/libexec/configureLocalKDC`

## Using a custom certificate

The certificate generated by `/usr/libexec/configureLocalKDC` has a duplicate extension, which causes Java's SSL to print a warning message whenever it loads the System certificate store. You can correct this problem by generating your own (valid) certificate that works with Java and matches OS X's expectations.

The script `generate-and-install-kdc-cert.sh` will use homebrew openssl to create a self-signed certificate and a public/private keypair, then install these to the System keychain using `security` and register it as a system identity using the `registerKDCCertificate` program (derived from Apple's `configureLocalKDC`). Your freshly generated openssl certificate will then be picked up by Apple's `configureLocalKDC`, which will use it when generating a new KDC database (rather than generating a new and technically wrong certificate).
