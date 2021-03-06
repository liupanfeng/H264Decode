//
// Created by ms on 2022/6/6.
//
#include "Codec.h"

char * msg= new char[64];

char * Codec::getInfo(int key) {
    return (char *) avcodec_configuration();
}

void Codec::init() {
    av_register_all();
    av_log(NULL,AV_LOG_DEBUG,"register success");
}

void Codec::release() {

}

void Codec::log(int ret, const char * func) {
    av_strerror(ret,msg,64);
    av_log(NULL,AV_LOG_ERROR,"%s error: %d,%s",func,ret,msg);
}

void Codec::set(int key, int value) {

}

int Codec::get(int key) {
    return 0;
}

int Codec::input(uint8_t *data) {
    return 0;
}

int Codec::output(uint8_t *data) {
    return 0;
}
