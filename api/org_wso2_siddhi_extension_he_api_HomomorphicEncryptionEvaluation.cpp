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

JNIEXPORT jboolean JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_compareGreaterThanIntInt
  (JNIEnv * env, jobject jobj, jint val1, jint val2)
{
    printf("Executing greater than on leftOp[%d] and rightOp[%d]\n", val1, val2);
    // set the number of GPUs to use
    multiGPUs(1);
    // initialize HE scheme
    dhs = new CuDHS(5, p, 1, 61, 20, 8191);
    // set random seed
    SetSeed(to_ZZ(time(NULL)));
    checkAnd();
    delete dhs;
    return val1 > val2;
}

void checkAnd() {
  ZZX x[2], y[2];
  for (int k=0; k<2; k++) {
    for (int i=0; i<dhs->numSlot(); i++)
      SetCoeff(x[k], i, RandomBnd(p));
    dhs->batcher->encode(y[k], x[k]);
    dhs->encrypt(y[k], y[k], 0);
  }

  cout<<"x0"<<x[0];
  cout<<"x1"<<x[1];
  cout<<"y0"<<y[0];
  cout<<"y1"<<y[1];

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

  ZZX chk;
  clear(chk);
  for (int i=0; i<dhs->numSlot(); i++)
    SetCoeff(chk, i, coeff(x[0], i)*coeff(x[1], i)%to_ZZ(p));

  cout<<"and\t";
  if (z == chk)
    cout<<"right"<<endl;
  else
    cout<<"wrong"<<endl;
  return;
}