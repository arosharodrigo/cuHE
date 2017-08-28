#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <sstream>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "org_wso2_siddhi_extension_he_api_HomomorphicEncDecService.h"
#include "DHS.h"

const int p = 2; // set message space to {0, 1}
CuDHS *dhs;

void init() {
	// initialize HE scheme
	dhs = new CuDHS(5, p, 1, 61, 20, 8191);
	// set random seed
	SetSeed(to_ZZ(time(NULL)));
}

void destroy() {
	delete dhs;
}

string compress(const std::string& data) {
	namespace bio = boost::iostreams;
	std::stringstream compressed;
	std::stringstream origin(data);
	bio::filtering_streambuf<bio::input> out;
	out.push(bio::gzip_compressor(bio::gzip_params(bio::gzip::best_compression)));
	out.push(origin);
	bio::copy(out, compressed);
	return compressed.str();
}

string decompress(const std::string& data) {
	namespace bio = boost::iostreams;
	std::stringstream compressed(data);
	std::stringstream decompressed;
	bio::filtering_streambuf<bio::input> out;
	out.push(bio::gzip_decompressor());
	out.push(compressed);
	bio::copy(out, decompressed);
	return decompressed.str();
}

JNIEXPORT void JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncDecService_init
  (JNIEnv * env, jobject jobj) {
	init();
}

JNIEXPORT void JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncDecService_destroy
  (JNIEnv * env, jobject jobj) {
	destroy();
}

JNIEXPORT jbyteArray JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncDecService_encrypt
  (JNIEnv * env, jobject jobj, jstring binaryForm32) {
	int bitLength = 32;
	const char *cstr = env->GetStringUTFChars(binaryForm32, NULL);

	ZZX x, y;
	for (int i=0; i<bitLength; i++) {
		SetCoeff(x, i, (int)cstr[i]-48);
	}

	env->ReleaseStringUTFChars(binaryForm32, cstr);

	dhs->batcher->encode(y, x);
	dhs->encrypt(y, y, 0);

	ostringstream ss;
	for (int i = 0; i < 8189; i++) {
		ss << y[i] << ",";
	}
	ss << y[8189];
	
	string str = ss.str();
//	jstring orb_string = env->NewStringUTF(str.c_str());
	
	int byteCount = str.length();
//	jbyte* pNativeMessage = reinterpret_cast<const jbyte*>(ss.str().c_str());
	jbyte* pNativeMessage = (jbyte*)str.c_str();
//	jbyte* pNativeMessage = (jbyte*)cstr;
	jbyteArray bytes = env->NewByteArray(byteCount);
	env->SetByteArrayRegion(bytes, 0, byteCount, pNativeMessage);

	return bytes;
}

JNIEXPORT jstring JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncDecService_decrypt
  (JNIEnv * env, jobject jobj, jstring encryptedVal) {
	const char *cstr = env->GetStringUTFChars(encryptedVal, NULL);
	std::vector<std::string> words;
	std::string s;
	boost::split(words, cstr, boost::is_any_of(","), boost::token_compress_on);

	ZZX encryptedNumber;
	encryptedNumber.SetLength(8192);
	encryptedNumber.SetMaxLength(8192);

	for (int k=0; k<words.size(); k++) {
		SetCoeff(encryptedNumber, k, to_ZZ(words[k].c_str()));
	}

	ZZX decryptedNumber;
	decryptedNumber.SetLength(8192);
	decryptedNumber.SetMaxLength(8192);
	dhs->decrypt(decryptedNumber, encryptedNumber, 1);
	dhs->batcher->decode(decryptedNumber, decryptedNumber);

	ostringstream ss;
	for (int i = 0; i < decryptedNumber.rep.length(); i++) {
		ss << decryptedNumber[i];
	}

	jstring orb_string = env->NewStringUTF(ss.str().c_str());
	return orb_string;
}

