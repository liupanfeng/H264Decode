package com.meishe.h264decode;

/**
 * * All rights reserved,Designed by www.meishesdk.com
 *
 * @Author : lpf
 * @CreateDate : 2022/6/6 下午2:12
 * @Description :
 * @Copyright :www.meishesdk.com Inc.All rights reserved.
 */
public class DecodeEngine {
    static {
        System.loadLibrary("native-lib");
    }

    /**
     * 得到解码数据
     * @return
     */
    public static native String getInfo();

    /**
     * 初始化
     */
    public static native void init();

    /**
     * 开始
     * @return
     */
    public native int start(String filePath);

    /**
     * 输入
     * @param data
     * @return
     */
    public native int input(byte[] data);

    /**
     * 输出
     * @param data
     * @return
     */
    public native int output(byte[] data);

    /**
     * 停止
     * @return
     */
    public native int stop();

    /**
     * 设置
     * @param key
     * @param value
     */
    public native void set(int key,int value);

    /**
     * 获取
     * @param key
     * @return
     */
    public native int get(int key);

    /**
     * 释放
     */
    public native void release();


}
