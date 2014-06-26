/*
 * Copyright (c) 2002-2003 Apple Computer, Inc. All Rights Reserved.
 *
 * The contents of this file constitute Original Code as defined in and are
 * subject to the Apple Public Source License Version 1.2 (the 'License').
 * You may not use this file except in compliance with the License. Please
 * obtain a copy of the License at http://www.apple.com/publicsource and
 * read it before using this file.
 *
 * This Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 */

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFDictionary.h>
#include <Security/Security.h>
#include <Security/SecCertificate.h>
#include <Security/SecItem.h>

#define KC_DB_PATH			"Library/Keychains"		/* relative to home */
#define SYSTEM_KDC			"com.apple.kerberos.kdc"

void printerr(const char* msg, OSStatus err)
{
    char buffer[1024];
    CFStringRef errmsg = SecCopyErrorMessageString(err, NULL);
    CFStringGetCString(errmsg, buffer, 1024, kCFStringEncodingUTF8);
    printf("%s:\n\t%s\n", msg, buffer);
}

void printcfs(const char* msg, CFStringRef body)
{
    char buffer[1024];
    CFStringGetCString(body, buffer, 1024, kCFStringEncodingUTF8);
    printf("%s:\n\t%s\n", msg, buffer);
}

int main (int argc, char **argv)
{
    OSStatus ortn;
    SecKeychainRef kcRef = NULL;
    
    CFStringRef cfDomain = CFStringCreateWithCString(NULL, SYSTEM_KDC, kCFStringEncodingASCII);
    
    const char *sysKcPath = "/Library/Keychains/System.keychain";
    ortn = SecKeychainOpen(sysKcPath, &kcRef);
    if(ortn) { printerr("Error opening keychain", ortn); exit(1); }
    
    SecCertificateRef certRef = SecCertificateCopyPreferred(cfDomain, NULL);
    
    if (certRef == NULL) {
        const void *keys[] = { kSecClass, kSecMatchSubjectContains, kSecReturnRef };
        const void *values[] = { kSecClassCertificate, cfDomain, kCFBooleanTrue };
        CFDictionaryRef query = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 3, NULL, NULL);
        
        CFTypeRef result = NULL;
        ortn = SecItemCopyMatching(query, &result);
        CFRelease(query);
        if (ortn || (result == NULL)) { printerr("Cannot find certificate!", ortn); exit(1); }
        
        certRef = (SecCertificateRef) result;
    }
    
    CFStringRef cn = NULL;
    ortn = SecCertificateCopyCommonName(certRef, &cn);
    if (ortn) { printerr("Cannot read common name from certificate!", ortn); exit(1); }
    printcfs("Found certicate", cn);
    printcfs("Description", SecCertificateCopyLongDescription(kCFAllocatorDefault, certRef, NULL));
    
    SecIdentityRef idRef;
    ortn = SecIdentityCreateWithCertificate(kcRef, certRef, &idRef);
    if (ortn) { printerr("Cannot register Identity (SecIdentityCreateWithCertificate)", ortn); exit(1); }
    
    ortn = SecIdentitySetSystemIdentity(cfDomain, idRef);
    CFRelease(idRef);
    if (ortn) { printerr("Cannot register Identity (SecIdentitySetSystemIdentity)", ortn); exit(1); }
    
    printf("..identity registered for domain %s.\n", SYSTEM_KDC);
    
    CFRelease(cfDomain);
    return 0;
}
