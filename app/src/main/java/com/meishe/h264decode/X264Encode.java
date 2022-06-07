package com.meishe.h264decode;

/**
 * @author : lpf
 * @FileName: X264Encode
 * @Date: 2022/6/6 22:50
 * @Description:
 */
public class X264Encode {

    static {
        System.loadLibrary("native-lib");
    }

    private OnEncodeListener mOnEncodeListener;

    public void setOnEncodeListener(OnEncodeListener onEncodeListener) {
        mOnEncodeListener = onEncodeListener;
    }


    //////////////////////////JNI 方法///////////////////////////////

    public native int init_x264(int width, int height, String h264Path, @YUVFormat int format);
    public native int encode_x264_data(byte[] data);
    public native void release_x264();
    /*进行编码 将yuv 转成 h264 */
    public native int encode_x264(int width, int height, String yuvPath, String h264Path, @YUVFormat int format);

    //////////////////////////////////////////////////////////////////////

    //    public native int init_aac(int sample_rate, int channel, int bitrate, String aac_path);
//    public native int encode_aac_data(byte[] data);
//    public native void release_aac();

    public void onEncodeH264(byte[] bytes, int size) {
        if (mOnEncodeListener != null) {
            mOnEncodeListener.onEncodeH264(bytes, size);
        }
    }

    public void onEncodeAAC(byte[] bytes, int size) {
        if (mOnEncodeListener != null) {
            mOnEncodeListener.onEncodeAAC(bytes, size);
        }
    }

    /**
     * 编码监听
     */
    public interface OnEncodeListener {
        void onEncodeH264(byte[] bytes, int size);

        void onEncodeAAC(byte[] bytes, int size);
    }

}
