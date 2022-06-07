//
// Created by ms on 2022/6/6.
//

#ifndef H264DECODE_H264_DECODER_H
#define H264DECODE_H264_DECODER_H

#include "Codec.h"

/***
 * 视频解码
 */
class H264_decoder : public Codec{
private:
    int width;
    int height;

    /*y分量大小*/
    size_t yFrameSize;
    /*uv分量大小*/
    size_t uvFrameSize;

public:
    int start(char * path);
    int input(uint8_t * data);
    int output(uint8_t * data);
    void set(int key,int value);
    int get(int key);
    int stop();

};


#endif //H264DECODE_H264_DECODER_H
