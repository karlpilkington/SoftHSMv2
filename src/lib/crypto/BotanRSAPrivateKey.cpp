/*
 * Copyright (c) 2010 .SE (The Internet Infrastructure Foundation)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*****************************************************************************
 BotanRSAPrivateKey.cpp

 Botan RSA private key class
 *****************************************************************************/

#include "config.h"
#include "log.h"
#include "BotanRSAPrivateKey.h"
#include "BotanUtil.h"
#include "BotanRNG.h"
#include "BotanCryptoFactory.h"
#include <string.h>
#include <botan/pkcs8.h>
#include <botan/pkcs8.h>
#include <botan/ber_dec.h>
#include <botan/der_enc.h>
#include <botan/oids.h>

// Constructors
BotanRSAPrivateKey::BotanRSAPrivateKey()
{
	rsa = NULL;
}

BotanRSAPrivateKey::BotanRSAPrivateKey(const Botan::RSA_PrivateKey* inRSA)
{
	BotanRSAPrivateKey();

	setFromBotan(inRSA);
}

// Destructor
BotanRSAPrivateKey::~BotanRSAPrivateKey()
{
	delete rsa;
}

// The type
/*static*/ const char* BotanRSAPrivateKey::type = "Botan RSA Private Key";

// Set from Botan representation
void BotanRSAPrivateKey::setFromBotan(const Botan::RSA_PrivateKey* rsa)
{
	ByteString p = BotanUtil::bigInt2ByteString(rsa->get_p());
	setP(p);
	ByteString q = BotanUtil::bigInt2ByteString(rsa->get_q());
	setQ(q);
	ByteString dp1 = BotanUtil::bigInt2ByteString(rsa->get_d1());
	setDP1(dp1);
	ByteString dq1 = BotanUtil::bigInt2ByteString(rsa->get_d2());
	setDQ1(dq1);
	ByteString pq = BotanUtil::bigInt2ByteString(rsa->get_c());
	setPQ(pq);
	ByteString d = BotanUtil::bigInt2ByteString(rsa->get_d());
	setD(d);
	ByteString n = BotanUtil::bigInt2ByteString(rsa->get_n());
	setN(n);
	ByteString e = BotanUtil::bigInt2ByteString(rsa->get_e());
	setE(e);
}

// Check if the key is of the given type
bool BotanRSAPrivateKey::isOfType(const char* type)
{
	return !strcmp(BotanRSAPrivateKey::type, type);
}

// Setters for the RSA private key components
void BotanRSAPrivateKey::setP(const ByteString& p)
{
	RSAPrivateKey::setP(p);

	if (rsa)
	{
		delete rsa;
		rsa = NULL;
	}
}

void BotanRSAPrivateKey::setQ(const ByteString& q)
{
	RSAPrivateKey::setQ(q);

	if (rsa)
	{
		delete rsa;
		rsa = NULL;
	}
}

void BotanRSAPrivateKey::setPQ(const ByteString& pq)
{
	RSAPrivateKey::setPQ(pq);

	if (rsa)
	{
		delete rsa;
		rsa = NULL;
	}
}

void BotanRSAPrivateKey::setDP1(const ByteString& dp1)
{
	RSAPrivateKey::setDP1(dp1);

	if (rsa)
	{
		delete rsa;
		rsa = NULL;
	}
}

void BotanRSAPrivateKey::setDQ1(const ByteString& dq1)
{
	RSAPrivateKey::setDQ1(dq1);

	if (rsa)
	{
		delete rsa;
		rsa = NULL;
	}
}

void BotanRSAPrivateKey::setD(const ByteString& d)
{
	RSAPrivateKey::setD(d);

	if (rsa)
	{
		delete rsa;
		rsa = NULL;
	}
}


// Setters for the RSA public key components
void BotanRSAPrivateKey::setN(const ByteString& n)
{
	RSAPrivateKey::setN(n);

	if (rsa)
	{
		delete rsa;
		rsa = NULL;
	}
}

void BotanRSAPrivateKey::setE(const ByteString& e)
{
	RSAPrivateKey::setE(e);

	if (rsa)
	{
		delete rsa;
		rsa = NULL;
	}
}

// Encode into PKCS#8 DER
ByteString BotanRSAPrivateKey::PKCS8Encode()
{
	ByteString der;
	createBotanKey();
	if (rsa == NULL) return der;
#if BOTAN_VERSION_MINOR == 11
	const Botan::secure_vector<Botan::byte> ber = Botan::PKCS8::BER_encode(*rsa);
#else
	const Botan::SecureVector<Botan::byte> ber = Botan::PKCS8::BER_encode(*rsa);
#endif
	der.resize(ber.size());
	memcpy(&der[0], &ber[0], ber.size());
	return der;
}

// Decode from PKCS#8 BER
bool BotanRSAPrivateKey::PKCS8Decode(const ByteString& ber)
{
	Botan::DataSource_Memory source(ber.const_byte_str(), ber.size());
	if (source.end_of_data()) return false;
#if BOTAN_VERSION_MINOR == 11
	Botan::secure_vector<Botan::byte> keydata;
#else
	Botan::SecureVector<Botan::byte> keydata;
#endif
	Botan::AlgorithmIdentifier alg_id;
	Botan::RSA_PrivateKey* key = NULL;
	try
	{

		Botan::BER_Decoder(source)
		.start_cons(Botan::SEQUENCE)
			.decode_and_check<size_t>(0, "Unknown PKCS #8 version number")
			.decode(alg_id)
			.decode(keydata, Botan::OCTET_STRING)
			.discard_remaining()
		.end_cons();
		if (keydata.empty())
			throw Botan::Decoding_Error("PKCS #8 private key decoding failed");
		if (Botan::OIDS::lookup(alg_id.oid).compare("RSA"))
		{
			ERROR_MSG("Decoded private key not RSA");

			return false;
		}
		BotanRNG* rng = (BotanRNG*)BotanCryptoFactory::i()->getRNG();
		key = new Botan::RSA_PrivateKey(alg_id, keydata, *rng->getRNG());
		if (key == NULL) return false;

		setFromBotan(key);

		delete key;
	}
	catch (std::exception& e)
	{
		ERROR_MSG("Decode failed on %s", e.what());

		return false;
	}

	return true;
}

// Retrieve the Botan representation of the key
Botan::RSA_PrivateKey* BotanRSAPrivateKey::getBotanKey()
{
	if (!rsa)
	{
		createBotanKey();
	}

	return rsa;
}

// Create the Botan representation of the key
void BotanRSAPrivateKey::createBotanKey()
{
	// d and n is not needed, they can be calculated
	if (this->p.size() != 0 &&
	    this->q.size() != 0 &&
	    this->e.size() != 0)
	{
		if (rsa)
		{
			delete rsa;
			rsa = NULL;
		}

		try
		{
			BotanRNG* rng = (BotanRNG*)BotanCryptoFactory::i()->getRNG();
			rsa = new Botan::RSA_PrivateKey(*rng->getRNG(),
						BotanUtil::byteString2bigInt(this->p), 
						BotanUtil::byteString2bigInt(this->q), 
						BotanUtil::byteString2bigInt(this->e), 
						BotanUtil::byteString2bigInt(this->d), 
						BotanUtil::byteString2bigInt(this->n));
		}
		catch (...)
		{
			ERROR_MSG("Could not create the Botan private key");
		}
        }
}
