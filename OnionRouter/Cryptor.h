#pragma once

//In RSA and DH, the conventional variable names are specific letters so we're using 1 letter variables for those.

#include <sstream>
#include "rsa.h"
#include "integer.h"
#include "osrng.h"
#include "cryptlib.h"
#include "rijndael.h"
#include "modes.h"
#include "files.h"
#include "osrng.h"
#include "hex.h"
#include "dh.h"
#include "pssr.h"
#include "sha.h"

using CryptoPP::Integer;
using CryptoPP::byte;

const Integer p("0xB10B8F96A080E01DDE92DE5EAE5D54EC52C99FBCFB06A3C6"
	"9A6A9DCA52D23B616073E28675A23D189838EF1E2EE652C0"
	"13ECB4AEA906112324975C3CD49B83BFACCBDD7D90C4BD70"
	"98488E9C219A73724EFFD6FAE5644738FAA31A4FF55BCCC0"
	"A151AF5F0DC8B4BD45BF37DF365C1A65E68CFDA76D4DA708"
	"DF1FB2BC2E4A4371");

const Integer g("0xA4D1CBD5C3FD34126765A442EFB99905F8104DD258AC507F"
	"D6406CFF14266D31266FEA1E5C41564B777E690F5504F213"
	"160217B4B01B886A5E91547F9E2749F4D7FBD7D3B9A92EE1"
	"909D0D2263F80A76A6A24C087A091F531DBF0A0169B6A28A"
	"D662A4D18E73AFA32D779D5918D08BC8858F4DCEF97C2A24"
	"855E6EEB22B3B2E5");

const Integer q("0xF518AA8781A8DF278ABA4E7D64B7CB9D49462353");

//The Cryptor class isn't static and thus each client has its own Cryptor object.
class Cryptor {
private:
	//RNG:
	static CryptoPP::AutoSeededRandomPool prng;

	//RSA:
	CryptoPP::RSA::PublicKey rsaSignaturePublicKey;
	static CryptoPP::RSA::PrivateKey rsaSignaturePrivateKey;

	//AES key & iv:
	CryptoPP::SecByteBlock m_aesKey;
	mutable CryptoPP::SecByteBlock m_aesIv;

	//Diffie-Hellman:
	CryptoPP::DH m_dh;
	CryptoPP::SecByteBlock m_dhPrivateKey;
	CryptoPP::SecByteBlock m_dhPublicKey;

public:
	//Constructor:
	Cryptor(void);

	//RSA:
	static const CryptoPP::SecByteBlock signMessageWithRsa(const CryptoPP::SecByteBlock& message); //Signs a message with the RSA private key.
	const bool verifyMessageWithRsa(const CryptoPP::SecByteBlock& message, const CryptoPP::SecByteBlock& signature); //Verifies a message signature with the RSA public key.
	void createRsaSignaturePublicKey(const Integer& n, const Integer& e); //Generates the public key from the public modulo and exponent.
	static void createRsaSignaturePrivateKey(void); //Generates the private key randomly.
	static const Integer& getSignaturePublicModulus(void); //Returns the public modulus.
	static const Integer& getSignaturePublicExponent(void); //Returns the public exponent.
	static const std::string signaturePublicKeyToHex(void); //Returns a hex string representing the public key.
	void hexToSignaturePublicKey(const std::string& hex); //Loads the public ket from a hex string.

	//AES:
	void createAesKey(void); //Generates AES 128-bit key.
	void createAesIv(void) const; //Generates AES initialisation vector.
	void setAesKey(const CryptoPP::SecByteBlock& key); //Sets the AES key.
	void setAesIv(const CryptoPP::SecByteBlock& iv) const; //Sets the AES IV.
	const CryptoPP::SecByteBlock& getAesIv(void) const; //Returns the AES IV.
	const CryptoPP::SecByteBlock& getAesKey(void) const; //Returns the AES key.
	const CryptoPP::SecByteBlock encryptAesMessage(const CryptoPP::SecByteBlock& message) const; //Encrypts the given message using the AES key and IV.
	const CryptoPP::SecByteBlock decryptAesMessage(const CryptoPP::SecByteBlock& message) const; //Decrypts the given encrypted message using the AES key and IV.


	//Diffie-Hellman:
	void createDhKeys(void); //Creates the Diffie-Hellman key.
	void createDhSharedKey(const CryptoPP::SecByteBlock& dhPublicKeyOther); //Creates the final shared key.
	const CryptoPP::SecByteBlock& getDhPublicKey(void) const; //Returns the Diffie-Hellman public key.
};