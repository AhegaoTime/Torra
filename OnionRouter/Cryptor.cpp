#include "Cryptor.h"

CryptoPP::AutoSeededRandomPool Cryptor::prng;
CryptoPP::RSA::PrivateKey Cryptor::rsaSignaturePrivateKey;

Cryptor::Cryptor(void) : m_aesKey(CryptoPP::AES::DEFAULT_KEYLENGTH), m_aesIv(CryptoPP::AES::BLOCKSIZE) {}

//Signs a message with the RSA pribate key.
const CryptoPP::SecByteBlock Cryptor::signMessageWithRsa(const CryptoPP::SecByteBlock& message) {
	//Create signer object.
	CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA256>::Signer signer(Cryptor::rsaSignaturePrivateKey);

	//Create signature space.
	size_t length = signer.MaxSignatureLength();
	CryptoPP::SecByteBlock signature(length);

	//Sign the message.
	length = signer.SignMessage(Cryptor::prng, message, message.size(), signature);

	//Resize now we know the true size of the signature.
	signature.resize(length);

	return signature;
}

//Verifies a message signature with the RSA public key.
const bool Cryptor::verifyMessageWithRsa(const CryptoPP::SecByteBlock& message, const CryptoPP::SecByteBlock& signature) {
	//Create verifier object.
	CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA256>::Verifier verifier(this->rsaSignaturePublicKey);

	//Verify the message.
	return verifier.VerifyMessage(message, message.size(), signature, signature.size());
}

//Generates the public key from the public modulo and exponent.
void Cryptor::createRsaSignaturePublicKey(const Integer& n, const Integer& e) {
	this->rsaSignaturePublicKey.Initialize(n, e);

	//Check key.
	if (!this->rsaSignaturePublicKey.Validate(Cryptor::prng, 3)) {
		throw std::exception("Rsa public key validation failed");
	}
}

//Generates the private key randomly.
void Cryptor::createRsaSignaturePrivateKey(void) {
	Cryptor::rsaSignaturePrivateKey.GenerateRandomWithKeySize(Cryptor::prng, 2048); //The encryption works only for a maximum bytes equal to the parameter (128 bits currently). 

	//Check key.
	if (!Cryptor::rsaSignaturePrivateKey.Validate(Cryptor::prng, 3)) {
		throw std::exception("Rsa private key validation failed");
	}
}

//Returns the public modulus.
const Integer& Cryptor::getSignaturePublicModulus(void) {
	return Cryptor::rsaSignaturePrivateKey.GetModulus();
}

//Returns the public exponent.
const Integer& Cryptor::getSignaturePublicExponent(void) {
	return Cryptor::rsaSignaturePrivateKey.GetPublicExponent();
}

//Returns a hex string representing the public key.
const std::string Cryptor::signaturePublicKeyToHex(void) {
	std::stringstream ss;
	ss << std::hex << Cryptor::rsaSignaturePrivateKey.GetModulus() << "|" << std::hex << Cryptor::rsaSignaturePrivateKey.GetPublicExponent();
	return ss.str();
}

//Loads the public ket from a hex string.
void Cryptor::hexToSignaturePublicKey(const std::string& hex) {
	const size_t separatorIndex = hex.find('|');
	if (separatorIndex == std::string::npos) {
		throw std::runtime_error("String does not contain '|'");
	}
	const std::string modulusString = hex.substr(0, separatorIndex);
	const std::string exponentString = hex.substr(separatorIndex + 1, hex.size() - separatorIndex - 1);

	const Integer n(modulusString.c_str());
	const Integer e(exponentString.c_str());

	this->createRsaSignaturePublicKey(n, e);
}

//Generates AES 128-bit key.
void Cryptor::createAesKey(void) {
	this->prng.GenerateBlock(this->m_aesKey, this->m_aesKey.size());
}

//Generates AES initialisation vector.
void Cryptor::createAesIv(void) const {
	this->prng.GenerateBlock(this->m_aesIv, this->m_aesIv.size());
}

//Sets the AES key.
void Cryptor::setAesKey(const CryptoPP::SecByteBlock& key) {
	this->m_aesKey = key;
}

//Sets the AES IV.
void Cryptor::setAesIv(const CryptoPP::SecByteBlock& iv) const {
	this->m_aesIv = iv;
}

//Returns the pointer of the AES IV.
const CryptoPP::SecByteBlock& Cryptor::getAesIv(void) const {
	return this->m_aesIv;
}

//Returns the AES key.
const CryptoPP::SecByteBlock& Cryptor::getAesKey(void) const {
	return this->m_aesKey;
}

//Encrypts the given message using the AES key and IV.
const CryptoPP::SecByteBlock Cryptor::encryptAesMessage(const CryptoPP::SecByteBlock& message) const {
	CryptoPP::SecByteBlock encryptedMessage;

	try {
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc;
		enc.SetKeyWithIV(this->m_aesKey, this->m_aesKey.size(), this->m_aesIv, this->m_aesIv.size());

		// Make room for padding
		encryptedMessage.resize(message.size() + CryptoPP::AES::BLOCKSIZE);
		CryptoPP::ArraySink cs(&encryptedMessage[0], encryptedMessage.size());

		CryptoPP::ArraySource a(message.data(), message.size(), true,
			new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(cs)));

		// Set cipher text length now that its known
		encryptedMessage.resize(cs.TotalPutLength());
	}
	catch (const CryptoPP::Exception& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	return encryptedMessage;
}

//Decrypts the given encrypted message using the AES key and IV.
const CryptoPP::SecByteBlock Cryptor::decryptAesMessage(const CryptoPP::SecByteBlock& encryptedMessage) const {
	CryptoPP::SecByteBlock recovered;

	try {
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption dec;
		dec.SetKeyWithIV(this->m_aesKey, this->m_aesKey.size(), this->m_aesIv, this->m_aesIv.size());

		// Recovered text will be less than cipher text
		recovered.resize(encryptedMessage.size());
		CryptoPP::ArraySink rs(&recovered[0], recovered.size());

		CryptoPP::ArraySource a(encryptedMessage.data(), encryptedMessage.size(), true,
			new CryptoPP::StreamTransformationFilter(dec, new CryptoPP::Redirector(rs)));

		// Set recovered text length now that its known
		recovered.resize(rs.TotalPutLength());
	}
	catch (const CryptoPP::Exception& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	return recovered;
}

//Creates the Diffie-Hellman public key.
void Cryptor::createDhKeys(void) {
	this->m_dh.AccessGroupParameters().Initialize(p, q, g);
	if (!this->m_dh.GetGroupParameters().ValidateGroup(this->prng, 3)) {
		throw std::runtime_error("Failed to validate prime and generator");
	}

	this->m_dhPrivateKey.resize(this->m_dh.PrivateKeyLength());
	this->m_dhPublicKey.resize(this->m_dh.PublicKeyLength());

	this->m_dh.GenerateKeyPair(this->prng, this->m_dhPrivateKey, this->m_dhPublicKey);
}

//Creates the final shared key.
void Cryptor::createDhSharedKey(const CryptoPP::SecByteBlock& dhPublicKeyOther) {
	CryptoPP::SecByteBlock dhSharedKey(this->m_dh.AgreedValueLength());

	if (!this->m_dh.Agree(dhSharedKey, this->m_dhPrivateKey, dhPublicKeyOther)) {
		throw std::runtime_error("Failed to reach shared secret.");
	}

	this->m_aesKey.Assign(dhSharedKey.BytePtr(), CryptoPP::AES::DEFAULT_KEYLENGTH); //Set as the AES key.
}

//Returns the Diffie-Hellman public key.
const CryptoPP::SecByteBlock& Cryptor::getDhPublicKey(void) const {
	return this->m_dhPublicKey;
}