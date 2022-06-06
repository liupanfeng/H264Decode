//
// Created by ms on 2022/6/6.
//

#include "H264Decoder.h"


int H264Decoder::start() {
    /*264模型文件路径*/
    const char *test = "/sdcard/test.264";
    avFormatContext = avformat_alloc_context();
    int ret = avformat_open_input(&avFormatContext, test, nullptr, nullptr);
    if (ret != 0) {
        log(ret, "avformat_open_input");
        return ret;
    }

    ret = avformat_find_stream_info(avFormatContext, nullptr);
    if (ret < 0) {
        log(ret, "avformat_find_stream_info");
        return ret;
    }
    /*得到解码器*/
    avCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    /*得到解码器上下文*/
    avCodecContext = avcodec_alloc_context3(avCodec);
    /*打开解码器*/
    ret = avcodec_open2(avCodecContext, avCodec, nullptr);
    if (ret != 0) {
        log(ret, "avcodec_open2");
        return ret;
    }
    /*得到封装格式*/
    avPacket = av_packet_alloc();
    /*初始化封装格式*/
    av_init_packet(avPacket);

    /*分配一个 AVFrame 并将其字段设置为默认值。
     * 必须使用 av_frame_free() 释放生成的结构。*/
    avFrame = av_frame_alloc();

    width = avFormatContext->streams[0]->codecpar->width;
    height = avFormatContext->streams[0]->codecpar->height;

    yFrameSize = width * height;
    uvFrameSize = yFrameSize >> 2;
    av_log(NULL, AV_LOG_DEBUG, "w,h,yframe,uvframe info:%d,%d,%d,%d", width, height, yFrameSize,
           uvFrameSize);
    av_log(NULL, AV_LOG_DEBUG, " start success");
    return 0;
}


int H264Decoder::input(uint8_t *data) {
    return 0;
}

int H264Decoder::output(uint8_t *data) {
    int ret = av_read_frame(avFormatContext, avPacket);
    if (ret != 0) {
        log(ret, "av_read_frame");
        return ret;
    }
    /*提供原始数据包数据作为解码器的输入*/
    ret = avcodec_send_packet(avCodecContext, avPacket);
    if (ret != 0) {
        log(ret, "avcodec_send_packet");
        return ret;
    }
    /*从解码器返回解码后的输出数据*/
    ret = avcodec_receive_frame(avCodecContext, avFrame);
    if (ret == 0) {
        memcpy(data, avFrame->data[0], yFrameSize);
        memcpy(data + yFrameSize, avFrame->data[1], uvFrameSize);
        memcpy(data + yFrameSize + uvFrameSize, avFrame->data[2], uvFrameSize);
    } else {
        log(ret, "avcodec_receive_frame");
    }

    av_packet_free(&avPacket);
    return ret;

}

int H264Decoder::stop() {
    avcodec_free_context(&avCodecContext);
    avformat_close_input(&avFormatContext);
}

int H264Decoder::get(int key) {
    switch (key) {
        case KEY_WIDTH:
            return width;
        case KEY_HEIGHT:
            return height;
        default:
            break;
    }
    return Codec::get(key);
}

void H264Decoder::set(int key, int value) {
    Codec::set(key, value);
}


