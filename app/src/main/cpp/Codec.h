//
// Created by ms on 2022/6/6.
//

#ifndef H264DECODE_CODEC_H
#define H264DECODE_CODEC_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
};

class Codec {

public:
    /*ffmpeg 格式上下文*/
    AVFormatContext * avFormatContext;
    /*ffmpeg 解码上下文*/
    AVCodecContext * avCodecContext;
    /*ffmpeg解码器*/
    AVCodec * avCodec;
    /*ffmpeg 封装格式*/
    AVPacket * avPacket;
    /*ffmpeg 帧格式*/
    AVFrame * avFrame;

public:
    /*宽*/
    static const int KEY_WIDTH=0x1001;
    /*高*/
    static const int KEY_HEIGHT=0x1002;
    /*码率*/
    static const int KEY_BIT_RATE=0x2001;
    /*样本率*/
    static const int KEY_SAMPLE_RATE=0x2002;
    /*音频规格*/
    static const int KEY_AUDIO_FORMAT=0x2003;
    /*轨道数量*/
    static const int KEY_CHANNEL_COUNT=0x2004;
    /*帧大小*/
    static const int KEY_FRAME_SIZE=0x2005;


public:
    /*得到char数组*/
    static char * getInfo(int key);
    /*初始化*/
    static void init();
    /*释放指针对象*/
    static void release();
    /*输出日志*/
    static void log(int ret, const char * func);
    /*开始*/
    virtual int start()=0;
    /*设置*/
    virtual void set(int key,int value);
    /*获取*/
    virtual int get(int key);
    /*输入*/
    virtual int input(uint8_t * data);
    /*输出*/
    virtual int output(uint8_t *data);
    /*停止*/
    virtual int stop()=0;
};


#endif //H264DECODE_CODEC_H
