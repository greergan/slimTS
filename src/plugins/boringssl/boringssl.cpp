//https://stackoverflow.com/questions/256405/programmatically-create-x509-certificate-using-openssl/15082282#15082282
#include <iostream>
#include <v8.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <slim/plugin.hpp>

namespace slim::boringssl {
	void create_self_signed_x509_certificate(const v8::FunctionCallbackInfo<v8::Value>& args);
	//create_x509_certificate();
	//create_private_key();
	//create_rsa_key();
};

/*
	param: country_code 
	param: organization_name
	param: host_name
	param: valid_when = 0 = now is offset in seconds
	param: valid_for_days = 60 seconds * 60 minutes * 24 hours * N days
	param: x509_certificate_serial_number
*/
#define X509_VERSION 3
void slim::boringssl::create_self_signed_x509_certificate(const v8::FunctionCallbackInfo<v8::Value>& args) {
	int x509_certificate_serial_number = 1;
	long valid_when = 0;
	long valid_for_days = 60 * 60 * 24 * 365;
	std::cout << "about to RSA_new()\n";
	RSA* rsa = RSA_new();
	std::cout << "about to BN_new()\n";
	BIGNUM* bignum = BN_new();
	std::cout << "about to BN_set_word()\n";
	if(BN_set_word(bignum, RSA_F4) == 1) {
		std::cout << "about to RSA_generate_key_ex()\n";
		if(RSA_generate_key_ex(rsa, 2048, bignum, NULL) == 1) {
			std::cout << "about to EVP_PKEY_new()\n";
			EVP_PKEY* pkey = EVP_PKEY_new();
			if(EVP_PKEY_assign_RSA(pkey, rsa) == 1) {
				std::cout << "about to X509_new()\n";
				X509* x509 = X509_new();
				std::cout << "about to X509_set_version()\n";
				if(X509_set_version(x509, X509_VERSION) == 1) {
					std::cout << "about to X509_get_serialNumber()\n";
					ASN1_INTEGER_set(X509_get_serialNumber(x509), x509_certificate_serial_number);
					std::cout << "about to X509_gmtime_adj()\n";
					X509_gmtime_adj(X509_get_notBefore(x509), valid_when);
					std::cout << "about to X509_gmtime_adj()\n";
					X509_gmtime_adj(X509_get_notAfter(x509), valid_for_days);
					std::cout << "about to X509_set_pubkey()\n";
					X509_set_pubkey(x509, pkey);
					std::cout << "about to X509_get_subject_name()\n";
					X509_NAME* name = X509_get_subject_name(x509);
					std::cout << "about to X509_NAME_add_entry_by_txt()\n";
					X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (unsigned char *)"CA", -1, -1, 0);
					std::cout << "about to X509_NAME_add_entry_by_txt()\n";
					X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (unsigned char *)"MyCompany Inc.", -1, -1, 0);
					std::cout << "about to X509_NAME_add_entry_by_txt()\n";
					X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"localhost", -1, -1, 0);
					std::cout << "about to X509_set_issuer_name()\n";
					X509_set_issuer_name(x509, name);
					std::cout << "about to X509_sign()\n";
					X509_sign(x509, pkey, EVP_sha1());
				}
				std::cout << "about to X509_free()\n";
				X509_free(x509);
			}
			std::cout << "about to EVP_PKEY_free()\n";
			EVP_PKEY_free(pkey);
		}
	}
	//std::cout << "about to RSA_free()\n";
	//RSA_free(rsa);
	std::cout << "about to BN_free()\n";
	BN_free(bignum);
}
void GenerateRSAKey(const std::vector<std::string> &args) {
	unsigned bits = 2048;
	bssl::UniquePtr<RSA> rsa(RSA_new());
	bssl::UniquePtr<BIGNUM> e(BN_new());
	bssl::UniquePtr<BIO> bio(BIO_new_fp(stdout, BIO_NOCLOSE));
	if(!BN_set_word(e.get(), RSA_F4) || !RSA_generate_key_ex(rsa.get(), bits, e.get(), NULL) ||
      		!PEM_write_bio_RSAPrivateKey(bio.get(), rsa.get(), NULL /* cipher */,
                                   NULL /* key */, 0 /* key len */,
                                   NULL /* password callback */,
                                   NULL /* callback arg */)) {
	}
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin boringssl_plugin(isolate, "boringssl");
	boringssl_plugin.add_function("create_self_signed_x509_certificate", slim::boringssl::create_self_signed_x509_certificate);
	boringssl_plugin.expose_plugin();
	return;
}