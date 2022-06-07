#include "JniTool.h"
#include <jni.h>
#include <string.h>
#include <malloc.h>
#include <x264.h>
#include "android/log.h"
#include "x264_encode.h"

#define LOG_TAG "lpf"

#define LOGE(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// Java虚拟机
JavaVM *java_vm;
// 公用环境变量
JNIEnv *jni_env;
// 类对象
jobject c_h264_obj;
jobject c_aac_obj;


void call_java_encode_h264(unsigned char *data, int size) {
    if (!c_h264_obj || !jni_env) {
        LOGE("Jni env is NULL, Check it!");
        return;
    }
    jclass cls = (*jni_env)->GetObjectClass(jni_env,c_h264_obj);
    if (!cls) {
        LOGE("Class is not found, Check it!");
        return;
    }
    // 获取到当前方法
    jmethodID method_id = (*jni_env)->GetMethodID(jni_env,cls, "onEncodeH264", "([BI)V");
    jbyteArray byte_array = (*jni_env)->NewByteArray(jni_env,size);
    (*jni_env)->SetByteArrayRegion(jni_env,byte_array, 0, size, (jbyte *)data);
    (*jni_env)->CallVoidMethod(jni_env,c_h264_obj, method_id, byte_array, size);
}



JNIEXPORT jint JNICALL
Java_com_meishe_h264decode_X264Encode_init_1x264(JNIEnv *env, jobject thiz, jint width, jint height,
                                                 jstring h264_path, jint yuv_csp) {
    jni_env = env;
    c_h264_obj = (*jni_env)->NewGlobalRef(env,thiz);
    const char *x264_file_path = (*env)->GetStringUTFChars(jni_env,h264_path, JNI_FALSE);
    /*初始化h264*/
    return x264_enc_init(width, height, x264_file_path, yuv_csp);
}

JNIEXPORT jint JNICALL
Java_com_meishe_h264decode_X264Encode_encode_1x264_1data(JNIEnv *env, jobject thiz,
                                                         jbyteArray data) {
    jbyte *bytes = (*env)->GetByteArrayElements(env,data, JNI_FALSE);
    int64_t size = (*env)->GetArrayLength(env,data);
//    char *buffer = new char[size];
    char *buffer = (char *)malloc(size);
    for (int i = 0; i < size; ++i) {
        buffer[i] = bytes[i];
    }

    LOGI("incoming yuv data size %ld", strlen(buffer));
    // 释放资源
    (*env)->ReleaseByteArrayElements(env,data, bytes, 0);

    /*进行编码操作*/
    int ret = x264_enc_data(buffer, size);
//    free buffer;
    free(buffer);
    return ret;
}

JNIEXPORT void JNICALL
Java_com_meishe_h264decode_X264Encode_release_1x264(JNIEnv *env, jobject thiz) {
    jni_env = env;
    (*jni_env)->DeleteGlobalRef(env,c_h264_obj);
    x264_enc_release();
}

/**
 * 将yuv 转成 h264
 *
 * @param env
 * @param thiz
 * @param width  视频的宽
 * @param height  视频高
 * @param yuv_path  yuv文件路径
 * @param h264_path  h264文件路径
 * @param yuv_csp  yuv 颜色空间
 * @return
 */
JNIEXPORT jint JNICALL
Java_com_meishe_h264decode_X264Encode_encode_1x264(JNIEnv *env, jobject thiz, jint width,
                                                   jint height, jstring yuv_path, jstring h264_path,
                                                   jint yuv_csp) {
    int ret = 0;
    if (width == 0 || height == 0) {
        LOGE("width or height cannot be zero!");
    }
    /* 将jni jstring数据 转成 char *数据 */
    const char *yuv_file_path = (*env)->GetStringUTFChars(env,yuv_path, JNI_FALSE);
    const char *h264_file_path = (*env)->GetStringUTFChars(env,h264_path, JNI_FALSE);

    /*对文件路径进行空判断*/
    if (!yuv_file_path) {
        LOGE("yuv path cannot be null");
        return -1;
    }
    if (!h264_file_path) {
        LOGE("h264 path cannot be null");
        return -1;
    }

    // 文件操作  打开yuv文件   "r":打开一个已有的文本文件，允许读取文件   如果处理的是二进制文件 则使用"rb"
    FILE *yuv_file = fopen(yuv_file_path, "rb");
    if (yuv_file == NULL) {
        LOGE("cannot open yuv file");
        return -1;
    }

    /*打开h264文件  打开一个文本文件，允许写入文件。如果文件不存在，
     * 则会创建一个新文件。如果处理的是二进制文件 则使用"wb" */
    FILE *h264_file = fopen(h264_file_path, "wb");
    if (h264_file == NULL) {
        LOGE("cannot open h264 file");
        return -1;
    }

    /*设置x264处理的yuv格式默认为YUV420*/
    int csp = X264_CSP_I420;
    switch (yuv_csp) {
        case 0:
            csp = X264_CSP_I420;
            break;
        case 1:
            csp = X264_CSP_NV21;
            break;
        case 2:
            csp = X264_CSP_I422;
            break;
        case 3:
            csp = X264_CSP_I444;
            break;
        default:
            csp = X264_CSP_I420;
    }

    LOGI("the params is success:\n %dx%d %s %s:", width, height, yuv_file_path, h264_file_path);

    int frame_number = 0;
    // 处理h264单元数据
    int i_nal = 0;
    x264_nal_t *nal = NULL;
    // x264
    x264_t *h = NULL;

    /*给编码参数 分配堆内存*/
    x264_param_t *param = (x264_param_t *) malloc(sizeof(x264_param_t));

    x264_picture_t *pic_in = (x264_picture_t *) (malloc(sizeof(x264_picture_t)));
    x264_picture_t *pic_out = (x264_picture_t *) (malloc(sizeof(x264_picture_t)));

    // 初始化编码参数
    x264_param_default(param);
    /*给编码参数赋值*/
    param->i_width = width;
    param->i_height = height;
    param->i_csp = csp;

    /*配置处理级别*/
    x264_param_apply_profile(param, x264_profile_names[5]);
    // 通过配置的参数打开编码器
    h = x264_encoder_open(param);
    /*初始化输出图片*/
    x264_picture_init(pic_out);
    /*给输入图片分配内存 */
    x264_picture_alloc(pic_in, param->i_csp, param->i_width, param->i_height);
    /*编码前每一帧的字节大小*/
    int size = param->i_width * param->i_height;

    // 计算视频帧数
    fseek(yuv_file, 0, SEEK_END);
    switch (csp) {
        case X264_CSP_I444:
            // YUV444
            frame_number = ftell(yuv_file) / (size * 3);
            break;
        case X264_CSP_I422:
            // YUV422
            frame_number = ftell(yuv_file) / (size * 2);
            break;
        case X264_CSP_I420:
            //YUV420
            frame_number = ftell(yuv_file) / (size * 3 / 2);
            break;
        case X264_CSP_NV21:
            //YUV420SP
            frame_number = ftell(yuv_file) / (size * 3 / 2);
            break;
        default:
            LOGE("Colorspace Not Support.");
            return -1;
    }
    fseek(yuv_file, 0, SEEK_SET);
    // 循环执行编码
    for (int i = 0; i < frame_number; i++) {
        switch (csp) {
            case X264_CSP_I444:
                fread(pic_in->img.plane[0], size, 1, yuv_file);
                fread(pic_in->img.plane[1], size, 1, yuv_file);
                fread(pic_in->img.plane[2], size, 1, yuv_file);
                break;
            case X264_CSP_I422:
                fread(pic_in->img.plane[0], size, 1, yuv_file);
                fread(pic_in->img.plane[1], size / 2, 1, yuv_file);
                fread(pic_in->img.plane[2], size / 2, 1, yuv_file);
                break;
            case X264_CSP_I420:
                /*将数据从文件读取 指定大小的数据 到平面指针中去*/
                fread(pic_in->img.plane[0], size, 1, yuv_file);
                fread(pic_in->img.plane[1], size / 4, 1, yuv_file);
                fread(pic_in->img.plane[2], size / 4, 1, yuv_file);
                break;
            case X264_CSP_NV21:
                // 只有两个planar，即是 y + vu
                fread(pic_in->img.plane[0], size, 1, yuv_file);
                fread(pic_in->img.plane[1], size / 2, 1, yuv_file);
                break;
        }
        pic_in->i_pts = i;
        /*对每一帧执行编码*/
        ret = x264_encoder_encode(h, &nal, &i_nal, pic_in, pic_out);
        if (ret < 0) {
            LOGE("x264 encode error");
            return -1;
        }
        LOGI("encode frame: %d\n", i);
        // 将编码数据循环写入目标文件
        for (int j = 0; j < i_nal; ++j) {
            if (nal[j].i_type == NAL_SPS) {
                LOGI("this nal is sps, count is: %d", nal[j].i_payload);
            }
            if (nal[j].i_type == NAL_PPS) {
                LOGI("this nal is pps, count is: %d", nal[j].i_payload);
            }
            /*第一个参数：这是指向要被写入元素的指针
             * 第二个参数：这是要被写入的每个元素的大小，以字节为单位
             * 第三个参数：这是元素的个数，每个元素的大小为 size 字节
             * 第四个参数：这是指向 FILE 对象的指针，该 FILE 对象指定了一个输出流。
             * */
            fwrite(nal[j].p_payload, 1, nal[j].i_payload, h264_file);
        }
    }

    // 冲刷缓冲区，不执行可能造成数据不完整
    int i = 0;
    while (1) {
        ret = x264_encoder_encode(h, &nal, &i_nal, NULL, pic_out);
        if (ret == 0) {
            break;
        }
        LOGD("flush %d frame", i);
        // 将编码数据循环写入目标文件
        for (int j = 0; j < i_nal; ++j) {
            fwrite(nal[j].p_payload, 1, nal[j].i_payload, h264_file);
        }
        i++;
    }

    x264_picture_clean(pic_in);
    x264_encoder_close(h);
    // 释放分配的空间
    free(pic_in);
    free(pic_out);
    free(param);
    // 关闭文件输入
    fclose(yuv_file);
    fclose(h264_file);

    return ret;
}