#include <jni.h>

#ifndef _Included_org_wso2_siddhi_extension_he_api_HomomorphicEncDecService
#define _Included_org_wso2_siddhi_extension_he_api_HomomorphicEncDecService
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncDecService_init
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncDecService_destroy
  (JNIEnv *, jobject);

JNIEXPORT jbyteArray JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncDecService_encrypt
  (JNIEnv *, jobject, jstring);

JNIEXPORT jstring JNICALL Java_org_wso2_siddhi_extension_he_api_HomomorphicEncDecService_decrypt
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
