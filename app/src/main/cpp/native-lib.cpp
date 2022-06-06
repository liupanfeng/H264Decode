#include <jni.h>
#include <string>
#include <iostream>
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"lpf",__VA_ARGS__)

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/jni.h>
}

#include <ffmpeg_log.h>
#include <ios>
#include "Codec.h"
#include "H264Decoder.h"

using namespace std;


Codec *codec;


extern "C" JNIEXPORT jstring JNICALL
Java_com_meishe_h264decode_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

/**
 *  初始化
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_meishe_h264decode_DecodeEngine_init(JNIEnv *env, jclass clazz) {
    av_log_set_callback(callback_report);
    /*初始化*/
    Codec::init();
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_meishe_h264decode_DecodeEngine_start(JNIEnv *env, jobject thiz) {
    codec=new H264Decoder();
    return codec->start();
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_meishe_h264decode_DecodeEngine_input(JNIEnv *env, jobject thiz, jbyteArray data) {
    return codec->input(reinterpret_cast<uint8_t *>(env->GetByteArrayElements(data, JNI_FALSE)));
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_meishe_h264decode_DecodeEngine_output(JNIEnv *env, jobject thiz, jbyteArray data) {
    return codec->output(reinterpret_cast<uint8_t *>(env->GetByteArrayElements(data, JNI_FALSE)));
}




extern "C"
JNIEXPORT jint JNICALL
Java_com_meishe_h264decode_DecodeEngine_get(JNIEnv *env, jobject thiz, jint key) {
    return codec->get(key);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_meishe_h264decode_DecodeEngine_stop(JNIEnv *env, jobject thiz) {
    return codec->stop();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_meishe_h264decode_DecodeEngine_set(JNIEnv *env, jobject thiz, jint key, jint value) {
    // TODO: implement set()
}




extern "C"
JNIEXPORT void JNICALL
Java_com_meishe_h264decode_DecodeEngine_release(JNIEnv *env, jobject thiz) {
    Codec::release();
}



extern "C"
JNIEXPORT jstring JNICALL
Java_com_meishe_h264decode_DecodeEngine_getInfo(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF(Codec::getInfo(0));
}