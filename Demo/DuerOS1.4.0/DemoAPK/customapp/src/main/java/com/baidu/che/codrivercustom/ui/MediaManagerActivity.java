package com.baidu.che.codrivercustom.ui;

import com.baidu.che.codrivercustom.util.LogUtil;
import com.baidu.che.codrivercustom.util.ToastUtils;
import com.baidu.che.codrivercustom.widget.FuncButton;
import com.baidu.che.codriversdk.manager.CdAsrManager;
import com.baidu.che.codriversdk.manager.CdMediaManager;
import com.baidu.che.codriversdk.manager.CdRecordManager;
import com.baidu.che.codriversdk.manager.CdTTSPlayerManager;

import android.os.Bundle;
import android.view.View;

/**
 * 多媒体管理类
 */

public class MediaManagerActivity extends BaseActivity {

    private static final String TAG = "MediaManagerActivity";
    private static final boolean DEFAULT_RESULT = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        /**
         * 必须先执行setMediaTool(...)/setRecordTool(...)/setPlayerTool(...),接入者才能接收到相关回调
         */
        addFunctionBtn(new FuncButton(mContext, "设置多媒体工具", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdMediaManager.getInstance().setMediaTool(mMediaTool);
                ToastUtils.show("设置多媒体工具");
            }
        }), new FuncButton(mContext, "设置录音控制工具", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdRecordManager.getInstance().setRecordTool(mRecordTool);
                ToastUtils.show("设置录音控制工具");
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "输送录音数据", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //数组长度根据实际情况确定
                byte[] rawData = new byte[1024];
                CdRecordManager.getInstance().feedRawAudioBuffer(rawData);
                ToastUtils.show("输送录音数据");
            }
        }), new FuncButton(mContext, "输送mic信号和spk信号录音数据", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //数组长度根据实际情况确定
                byte[] micData = new byte[1024];
                byte[] spkData = new byte[1024];
                CdRecordManager.getInstance().feedAudioBuffer(micData, spkData);
                ToastUtils.show("输送mic信号和spk信号录音数据");
            }
        }));

        addTitle(20, "设置AEC和录音相关的特性", null);

        addFunctionBtn(new FuncButton(mContext, "MIC_LEFT", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //目前支持的RecordType, 根据需要填写
                //内部录音,无AEC，单声道，只有Mic信号    INSIDE_RAW,
                //内部录音,有AEC，左声道是Mic，右声道是Speaker   INSIDE_AEC_MIC_LEFT,
                //内部录音,有AEC，右声道是Mic，左声道是Speaker   INSIDE_AEC_MIC_RIGHT,
                //外部录音,无AEC，单声道，只有Mic信号   OUTSIDE_RAW,
                //外部录音,有AEC，左声道是Mic，右声道是Speaker   OUTSIDE_AEC_MIC_LEFT,
                //外部录音,有AEC，右声道是Mic，左声道是Speaker   OUTSIDE_AEC_MIC_RIGHT,
                //外部录音,有AEC，厂商自己分离Mic和Speaker信号   OUTSIDE_AEC_DUAL_CHANNEL
                CdRecordManager.getInstance().
                        setRecordType(CdRecordManager.RecordType.INSIDE_AEC_MIC_LEFT);
                ToastUtils.show("设置type = MIC_LEFT");
            }
        }), new FuncButton(mContext, "MIC_RIGHT", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdRecordManager.getInstance().
                        setRecordType(CdRecordManager.RecordType.INSIDE_AEC_MIC_RIGHT);
                ToastUtils.show("设置type = MIC_RIGHT");
            }
        }));

    }

    private CdRecordManager.RecordTool mRecordTool = new CdRecordManager.RecordTool() {
        @Override
        public void startRecord() {
            LogUtil.d(TAG, "startRecord()");
            ToastUtils.show("开始录音");
        }

        @Override
        public void endRecord() {
            LogUtil.d(TAG, "endRecord()");
            ToastUtils.show("结束录音");
        }

        @Override
        public void initRecorder() {
            LogUtil.d(TAG, "initRecorders()");
            ToastUtils.show("录音初始化");
        }
    };

    private CdMediaManager.MediaTool mMediaTool = new CdMediaManager.MediaTool() {
        @Override
        public boolean openRadio() {
            LogUtil.d(TAG, "openRadio() ：true");
            ToastUtils.show("打开电台 ：true");
            return true;
        }

        @Override
        public boolean openMusicRadio() {
            LogUtil.d(TAG, "openMusicRadio() ：" + DEFAULT_RESULT);
            ToastUtils.show("打开音乐电台 ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeRadio() {
            LogUtil.d(TAG, "closeRadio() ：" + DEFAULT_RESULT);
            ToastUtils.show("关闭电台 ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean updateRadio() {
            LogUtil.d(TAG, "updateRadio() ：" + DEFAULT_RESULT);
            ToastUtils.show("更新电台 ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean nextRadio() {
            LogUtil.d(TAG, "nextRadio() ：" + DEFAULT_RESULT);
            ToastUtils.show("播放下一个电台 ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean preRadio() {
            LogUtil.d(TAG, "preRadio() ：" + DEFAULT_RESULT);
            ToastUtils.show("播放上一个电台 ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openFM() {
            LogUtil.d(TAG, "openFM() ：" + DEFAULT_RESULT);
            ToastUtils.show("打开FM ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openFMChannel(String s) {
            LogUtil.d(TAG, "openFMChannel() : " + s + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开指定频率的FM" + s + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openAM() {
            LogUtil.d(TAG, "openAM()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开AM" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;NOTIFY_FILTER_NONE
        }

        @Override
        public boolean openAMChannel(String s) {
            LogUtil.d(TAG, "openAMChannel() : " + s + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开指定频率的AM" + s + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openMusicUsb() {
            LogUtil.d(TAG, "openMusicUsb()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开USB音乐" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openMusicUsb1() {
            LogUtil.d(TAG, "openMusicUsb1()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开USB1音乐" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openMusicUsb2() {
            LogUtil.d(TAG, "openMusicUsb2()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开USB2音乐" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openMusicCd() {
            LogUtil.d(TAG, "openMusicCd()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开CD音乐" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openMusicAux() {
            LogUtil.d(TAG, "openMusicAux()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开AUX音乐" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openMusicIpod() {
            LogUtil.d(TAG, "openMusicIpod()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开ipod音乐" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openMusicBt() {
            LogUtil.d(TAG, "openMusicBt()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开蓝牙音乐" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeMusicBt() {
            LogUtil.d(TAG, "closeMusicBt()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("关闭蓝牙音乐" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openLocalMusic() {
            LogUtil.d(TAG, "openLocalMusic()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开本地音乐" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean playCollectionFM() {
            LogUtil.d(TAG, "playCollectionFM()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("播放收藏的电台" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean preCollectionFM() {
            LogUtil.d(TAG, "preCollectionFM()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("播放上一个收藏的电台" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean nextCollectionFM() {
            LogUtil.d(TAG, "nextCollectionFM()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("播放下一个收藏的电台" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean collectFMChannel() {
            LogUtil.d(TAG, "collectFMChannel()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("收藏当前电台" + " ：" + DEFAULT_RESULT);
            // 放FM打开状态查询的回调中
            if (true) {  // FM已经打开
                // 执行收藏的操作,厂商实现
                // 放收藏操作的回调中
                if (true) {
                    CdTTSPlayerManager.getInstance().play("好的,收藏了");
                } else {
                    CdTTSPlayerManager.getInstance().play("收藏电台失败");
                }
            } else {  // FM未打开
                CdTTSPlayerManager.getInstance().play("FM未打开,暂不支持该指令");
            }
            return DEFAULT_RESULT;
        }

        @Override
        public boolean cancelFMChannel() {
            LogUtil.d(TAG, "cancelFMChannel()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("取消收藏当前电台" + " ：" + DEFAULT_RESULT);
            // 放FM打开状态查询的回调中
            if (true) {  // FM已经打开
                // 执行取消收藏的操作,厂商实现
                // 放取消收藏操作的回调中
                if (true) {
                    CdTTSPlayerManager.getInstance().play("好的,已取消收藏");
                } else {
                    CdTTSPlayerManager.getInstance().play("取消收藏收藏电台失败");
                }
            } else {  // FM未打开
                CdTTSPlayerManager.getInstance().play("FM未打开,暂不支持该指令");
            }
            return DEFAULT_RESULT;
        }

        @Override
        public boolean searchAndRefreshFMChannel() {
            LogUtil.d(TAG, "searchAndRefreshFMChannel()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("更新电台列表" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openMusicSort(CdMediaManager.MusicSortType musicSortType) {
            LogUtil.d(TAG, "openMusicSort() : " + musicSortType.name() + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开音乐分类" + musicSortType.name() + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean playMusicList(CdMediaManager.MusicListType musicListType) {
            LogUtil.d(TAG, "playMusicList() : " + musicListType.name() + " ：" + DEFAULT_RESULT);
            ToastUtils.show("播放音乐列表" + musicListType.name() + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openVideo() {
            LogUtil.d(TAG, "openVideo()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("打开本地视频" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeVideo() {
            LogUtil.d(TAG, "closeVideo()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("关闭本地视频" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }
    };
}
