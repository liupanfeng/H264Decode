package com.meishe.h264decode;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.coremedia.iso.boxes.Container;
import com.googlecode.mp4parser.FileDataSourceImpl;
import com.googlecode.mp4parser.authoring.Movie;
import com.googlecode.mp4parser.authoring.builder.DefaultMp4Builder;
import com.googlecode.mp4parser.authoring.tracks.h264.H264TrackImpl;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;

/**
 * x264 编码页面
 */
public class H264EncodeActivity extends AppCompatActivity {

    private static final int REQUEST_CODE = 0x01;
    // x264编码
    private X264Encode mX264Encode;
    // yuv 和 h264路径
    private String yuvPath;
    private String h264Path;
    // 最终的mp4路径
    private String mp4Path;

    // 视频宽高
    private int width = 540;
    private int height = 960;

    private Button mExecuteBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_h264_encode);

        initView();
        initParams();
        initData();
        initListener();

    }


    private void initListener() {
        mExecuteBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                H264ActivityPermissionsDispatcher.showEncodeWithPermissionCheck(H264Activity.this);
                showEncode();
            }
        });
    }

    private void initView() {
        mExecuteBtn = findViewById(R.id.execute_btn);
    }

    private void initParams() {
        yuvPath = getExternalCacheDir() + File.separator + "test.yuv";
        h264Path = getExternalCacheDir() + File.separator + "test.h264";
        mp4Path = getExternalCacheDir() + File.separator + "test.mp4";

        new Thread(new Runnable() {
            @Override
            public void run() {
                FileUtils.copyFilesAssets(H264EncodeActivity.this, "test.yuv", yuvPath);
            }
        });

    }

    private void initData() {
        mX264Encode = new X264Encode();
    }

    //    @NeedsPermission({Manifest.permission.CAMERA,
//            Manifest.permission.RECORD_AUDIO,
//            Manifest.permission.WRITE_EXTERNAL_STORAGE})
    public void showEncode() {
        mX264Encode.encode_x264(width, height, yuvPath, h264Path, YUVFormat.YUV_420);
        h264ToMp4();
    }

    /**
     * 将h264转成mp4
     */
    private void h264ToMp4(){
        H264TrackImpl h264Track;
        try {
            h264Track = new H264TrackImpl(new FileDataSourceImpl(h264Path));
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
        Movie movie = new Movie();
        movie.addTrack(h264Track);

        Container mp4file = new DefaultMp4Builder().build(movie);

        FileChannel fc = null;
        try {
            fc = new FileOutputStream(new File(mp4Path)).getChannel();
            mp4file.writeContainer(fc);
            fc.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


}