#include <jni.h>

#ifndef _Included_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation
#define _Included_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_init
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_destroy
  (JNIEnv *, jobject);

JNIEXPORT jstring JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_compareEqualIntInt
  (JNIEnv *, jobject, jstring, jstring);

JNIEXPORT jstring JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation_compareGreaterThanIntInt
  (JNIEnv *, jobject, jstring, jstring);

void init();
void destroy();
int * convertToArray(int, int);

#ifdef __cplusplus
}
#endif
#endif
