#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cextdecs.h>
#include "com_hpe_nsk_nsdevops_server_JavaHelloServer.h"


/* All the JNI methods */

JNIEXPORT void JNICALL Java_com_hpe_nsk_nsdevops_server_JavaHelloServer_sayHelloNative
  (JNIEnv *env, jobject obj,  jstring input){
	char cname[16]="";
	int cnameLen = (*env)->GetStringLength(env, input);
      (*env)->GetStringUTFRegion(env, input, 0, cnameLen, cname);
	printf("hello");

}







	
