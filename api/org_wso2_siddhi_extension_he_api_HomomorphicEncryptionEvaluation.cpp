#include <jni.h>
#include <stdio.h>
#include "org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation.h"
#include "DHS.h"
#include "../cuhe/CuHE.h"
using namespace cuHE;
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>


const int p = 2; // set message space to {0, 1}
CuDHS *dhs;

JNIEXPORT jboolean JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_compareEqualIntInt
  (JNIEnv * env, jobject jobj, jint val1, jint val2)
{
	init();
    printf("Executing equality on leftOp[%d] and rightOp[%d]\n", val1, val2);
    int bitLength = 32;
    int i = 0;

	int * val1Array = convertToArray(val1, bitLength);
    for (i = 0; i < bitLength; ++i) {
    	cout << val1Array[i];
    }
    cout << "\n";

    int * val2Array = convertToArray(val2, bitLength);
    for (i = 0; i < bitLength; ++i) {
    	cout << val2Array[i];
    }
    cout << "\n";

    ZZX x[2], y[2];
    for (int k=0; k<2; k++) {
    	for (int i=0; i<bitLength; i++) {
    		SetCoeff(x[0], i, val1Array[i]);
    		SetCoeff(x[1], i, val2Array[i]);
    	}
    	dhs->batcher->encode(y[k], x[k]);
    	dhs->encrypt(y[k], y[k], 0);
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

    ZZX z;
    dhs->decrypt(z, cuz.zRep(), 1);
    dhs->batcher->decode(z, z);

    cout<<"Z - "<<z;
    cout << "\n";

    destroy();
    return z.rep.length() == 0;
}

JNIEXPORT jboolean JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_compareGreaterThanIntInt
  (JNIEnv * env, jobject jobj, jint val1, jint val2)
{
	init();
    printf("Executing greater than on leftOp[%d] and rightOp[%d]\n", val1, val2);
    int bitLength = 32;
    int i = 0;

	int * val1Array = convertToArray(val1, bitLength);
    for (i = 0; i < bitLength; ++i) {
    	cout << val1Array[i];
    }
    cout << "\n";

    int * val2Array = convertToArray(val2, bitLength);
    for (i = 0; i < bitLength; ++i) {
    	cout << val2Array[i];
    }
    cout << "\n";

    ZZX x[2], y[2];
    for (int k=0; k<2; k++) {
    	for (int i=0; i<bitLength; i++) {
    		SetCoeff(x[0], i, val1Array[i]);
    	    SetCoeff(x[1], i, val2Array[i]);
    	}
    	dhs->batcher->encode(y[k], x[k]);
    	dhs->encrypt(y[k], y[k], 0);
    }

    CuCtxt* cuy = new CuCtxt[2];
    CuCtxt cuz;
    for (int k=0; k<2; k++) {
    	cuy[k].setLevel(0, 0, y[k]);
        cuy[k].x2n();
    }
    cAnd(cuz, cuy[0], cuy[1]);
    delete [] cuy;
    cuz.relin();
    cuz.modSwitch();
    cuz.x2z();

    ZZX z;
    dhs->decrypt(z, cuz.zRep(), 1);
    dhs->batcher->decode(z, z);

    cout<<"Z - "<<z;
    cout << "\n";

    destroy();
    return val1 > val2;
}

void init() {
	// set the number of GPUs to use
	multiGPUs(1);
	// initialize HE scheme
	dhs = new CuDHS(5, p, 1, 61, 20, 8191);
	// set random seed
	SetSeed(to_ZZ(time(NULL)));
}

void destroy() {
	delete dhs;
}

int * convertToArray(int val, int bitLength) {
	int i;
	int* valArray = new int[bitLength];
	for (i = 0; i < bitLength; ++i) {
		valArray[bitLength-1-i] = val & (1 << i) ? 1 : 0;
	}
	return valArray;
}
