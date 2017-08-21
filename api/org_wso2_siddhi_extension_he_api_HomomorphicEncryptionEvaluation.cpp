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

#include "org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation.h"
#include "DHS.h"
#include "../cuhe/CuHE.h"
using namespace cuHE;

const int p = 2; // set message space to {0, 1}
CuDHS *dhs2;

void init() {
	// set the number of GPUs to use
	multiGPUs(1);
	// initialize HE scheme
	dhs2 = new CuDHS(5, p, 1, 61, 20, 8191);
	// set random seed
	SetSeed(to_ZZ(time(NULL)));
}

void destroy() {
	delete dhs2;
}

ZZX generateZZX(JNIEnv* env, jstring encryptedVal) {
	const char *cstr = env->GetStringUTFChars(encryptedVal, NULL);
	std::vector<std::string> words;
	std::string s;
	boost::split(words, cstr, boost::is_any_of(","), boost::token_compress_on);
	ZZX encryptedNumber;
	encryptedNumber.SetLength(8192);
	encryptedNumber.SetMaxLength(8192);
	for (int k = 0; k < words.size(); k++) {
		SetCoeff(encryptedNumber, k, to_ZZ(words[k].c_str()));
	}
	return encryptedNumber;
}

JNIEXPORT void JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_init
  (JNIEnv * env, jobject jobj) {
	init();
}

JNIEXPORT void JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_destroy
  (JNIEnv * env, jobject jobj) {
	destroy();
}

JNIEXPORT jstring JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_compareEqualIntInt
  (JNIEnv * env, jobject jobj, jstring val1, jstring val2) {
//	printf("Executing equality on leftOp[%d] and rightOp[%d]\n", val1, val2);
	printf("Executing equality\n");
	ZZX y[2];
	y[0] = generateZZX(env, val1);
	y[1] = generateZZX(env, val2);

	const char *cstr1 = env->GetStringUTFChars(val1, NULL);
	std::vector<std::string> words1;
	std::string s1;
	boost::split(words1, cstr1, boost::is_any_of(","), boost::token_compress_on);
	y[0].SetLength(8192);
	y[0].SetMaxLength(8192);
	for (int k = 0; k < words1.size(); k++) {
		SetCoeff(y[0], k, to_ZZ(words1[k].c_str()));
	}

	const char *cstr2 = env->GetStringUTFChars(val2, NULL);
	std::vector<std::string> words2;
	std::string s2;
	boost::split(words2, cstr2, boost::is_any_of(","), boost::token_compress_on);
	y[1].SetLength(8192);
	y[1].SetMaxLength(8192);
	for (int k = 0; k < words2.size(); k++) {
		SetCoeff(y[1], k, to_ZZ(words2[k].c_str()));
	}

    CuCtxt* cuy = new CuCtxt[2];
    CuCtxt cuz;
    for (int k=0; k<2; k++) {
    	cuy[k].setLevel(0, 0, y[k]);
        cuy[k].x2n();
    }
    cXor(cuz, cuy[0], cuy[1]);
    delete [] cuy;
    cuz.relin();
    cuz.modSwitch();
    cuz.x2z();

    ZZX result = y[0];
    ostringstream ss;
    for (int i = 0; i < 8189; i++) {
    	ss << result[i] << ",";
    }
    ss << result[8189];

    jstring orb_string = env->NewStringUTF(ss.str().c_str());
    return orb_string;
}

JNIEXPORT jstring JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_compareGreaterThanIntInt
  (JNIEnv * env, jobject jobj, jstring val1, jstring val2) {
    printf("Executing greater than on leftOp[%d] and rightOp[%d]\n", val1, val2);
    return val1;
}

int * convertToArray(int val, int bitLength) {
	int i;
	int* valArray = new int[bitLength];
	for (i = 0; i < bitLength; ++i) {
		valArray[bitLength-1-i] = val & (1 << i) ? 1 : 0;
	}
	return valArray;
}
