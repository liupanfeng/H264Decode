package com.meishe.h264decode;

import androidx.appcompat.app.AppCompatActivity;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;

import com.meishe.h264decode.databinding.ActivityH264DecoderBinding;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * x264 解码页面
 * @author ms
 */
public class H264DecoderActivity extends AppCompatActivity implements View.OnClickListener {

    private DecodeEngine mpeg;
    /**
     * 关注数据结构
     */
    private byte[] data;
    private boolean isCodecStarted = false;

    private ActivityH264DecoderBinding mBinding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mBinding = ActivityH264DecoderBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());
        /*初始化*/
        DecodeEngine.init();
        mpeg=new DecodeEngine();
        /*选择EGL使用的版本*/
        mBinding.mGLView.setEGLContextClientVersion(2);
        /*如果设置为 true，则在 GLSurfaceView 暂停时可以保留 EGL 上下文。*/
        mBinding.mGLView.setPreserveEGLContextOnPause(true);

        mBinding.mGLView.setRenderer(new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {

            }

            @Override
            public void onSurfaceChanged(GL10 gl10, int i, int i1) {

            }

            @Override
            public void onDrawFrame(GL10 gl10) {

            }
        });

        /*设置渲染模式。当 renderMode 为 RENDERMODE_CONTINUOUSLY 时，
        会重复调用渲染器重新渲染场景。当 renderMode 为 RENDERMODE_WHEN_DIRTY 时，
        渲染器仅在创建表面或调用 {@link requestRender} 时渲染。
        默认为 RENDERMODE_CONTINUOUSLY。*/
        mBinding.mGLView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

    }



    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.mBtnInfo:
                mBinding.mTvInfo.setText(DecodeEngine.getInfo());
                break;
            case R.id.mBtnStart:
               String path= new File(Environment.
                        getExternalStorageDirectory() +
                        File.separator + "T1.264")
                        .getAbsolutePath();

                mpeg.start(path);
                isCodecStarted=true;
                break;
            case R.id.mBtnDecode:
                mBinding.mGLView.requestRender();
                break;
            default:
                break;
        }
    }


    @Override
    protected void onResume() {
        super.onResume();
        if(mBinding.mGLView!=null){
            mBinding.mGLView.onResume();
        }
    }


    @Override
    protected void onPause() {
        super.onPause();
        if(mBinding.mGLView!=null){
            mBinding.mGLView.onPause();
        }
    }


    private byte[] InputStreamToByte(InputStream is) throws IOException {
        ByteArrayOutputStream bytestream = new ByteArrayOutputStream();
        int ch;
        while ((ch = is.read()) != -1) {
            bytestream.write(ch);
        }
        byte imgdata[] = bytestream.toByteArray();
        bytestream.close();
        return imgdata;
    }
}