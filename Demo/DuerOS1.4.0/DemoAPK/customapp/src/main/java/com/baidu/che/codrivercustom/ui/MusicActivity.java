package com.baidu.che.codrivercustom.ui;

import java.util.ArrayList;
import java.util.List;

import com.baidu.che.codrivercustom.util.LogUtil;
import com.baidu.che.codrivercustom.util.ToastUtils;
import com.baidu.che.codrivercustom.widget.FuncButton;
import com.baidu.che.codriversdk.manager.CdAsrManager;
import com.baidu.che.codriversdk.manager.CdMusicManager;

import android.os.Bundle;
import android.view.View;

/**
 * 音乐模块
 */

public class MusicActivity extends BaseActivity {

    private static final String TAG = "MusicActivity";
    private String[] names = {"吻别", "红玫瑰", "十年", "笨小孩", "江南", "一千年以后", "老街", "成都", "朋友", "后来"};
    private String[] singers = {"张学友", "陈奕迅", "陈奕迅", "刘德华", "林俊杰", "林俊杰", "李荣浩", "赵雷", "周华健", "刘若英"};

    private static final boolean DEFAULT_RESULT = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        /**
         * 接入该功能,必须先执行setMusicTool(...),接入者才能接收到相关回调
         */
        addFunctionBtn(new FuncButton(mContext, "设置音乐工具实例", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdMusicManager.getInstance().setMusicTool(mMusicTool);
                ToastUtils.show("设置音乐工具实例");
            }
        }));

        addTitle(20, "设置默认播放器", null);

        addFunctionBtn(new FuncButton(mContext, "百度随心听", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdMusicManager.getInstance().setMusicType(CdMusicManager.MusicType.BAIDU_RADIO);
                ToastUtils.show("设置默认播放器: 百度随心听");
            }
        }), new FuncButton(mContext, "酷我", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdMusicManager.getInstance().setMusicType(CdMusicManager.MusicType.KUWO_MUSIC);
                ToastUtils.show("设置默认播放器: 酷我");
            }
        }), new FuncButton(mContext, "QQ", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdMusicManager.getInstance().setMusicType(CdMusicManager.MusicType.QQ_MUSIC);
                ToastUtils.show("设置默认播放器: QQ");
            }
        }));
        addFunctionBtn(new FuncButton(mContext, "同步音乐列表", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                List<CdMusicManager.MusicModel> list = new ArrayList<>();
                for (int i = 0; i < names.length; i++) {
                    CdMusicManager.MusicModel musicModel = new CdMusicManager.MusicModel();
                    musicModel.name = names[i];
                    musicModel.albumArtistName = singers[i];
                    list.add(musicModel);
                }
                CdMusicManager.getInstance().syncLocalMusicData(list);
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "上一曲", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdMusicManager.getInstance().prev();
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "通知dueros本地播放器开始播放", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdMusicManager.getInstance().notifyInUse(CdMusicManager.MusicType.CUSTOM_MUSIC);
            }
        }), new FuncButton(mContext, "通知dueros百度随心听开始播放", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdMusicManager.getInstance().notifyInUse(CdMusicManager.MusicType.BAIDU_RADIO);
            }
        }));
        addFunctionBtn(new FuncButton(mContext, "触发播放，切歌等操作的时候，是否拉起播放器到前台", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 拉起播放器 true  反之false
                CdMusicManager.getInstance().notifyNeedLaunchApp(true);
            }
        }));
    }

    private CdMusicManager.MusicTool mMusicTool = new CdMusicManager.MusicTool() {

        @Override
        public boolean searchMusic(CdMusicManager.MusicModel musicModel) {
            CdAsrManager.getInstance().closeDialog();
            ToastUtils.show("搜索音乐:" + musicModel + " ：" + DEFAULT_RESULT);
            LogUtil.d(TAG, "searchMusic() " + musicModel.toString() + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean play() {
            LogUtil.d(TAG, "play()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("play" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean pause() {
            LogUtil.d(TAG, "pause()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("pause" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean prev() {
            LogUtil.d(TAG, "prev()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("prev" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean next() {
            LogUtil.d(TAG, "next()" + " ：true");
            ToastUtils.show("next" + " ：true");
            return true;
        }

        @Override
        public boolean exit() {
            LogUtil.d(TAG, "exit()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("exit" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean switchMode(int i) {
            LogUtil.d(TAG, "switchMode() : " + i + " ：" + DEFAULT_RESULT);
            ToastUtils.show("switchMode " + i + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean change() {
            LogUtil.d(TAG, "change()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("change" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }
    };
}
