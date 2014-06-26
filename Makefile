registerKDCCertificate: registerKDCCertificate.cpp
	c++ -framework Security -framework CoreFoundation registerKDCCertificate.cpp -o registerKDCCertificate

.PHONY: clean

clean:
	rm -f registerKDCCertificate

