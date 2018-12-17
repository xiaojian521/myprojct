package com.baidu.che.codrivercustom.ui;

import com.baidu.che.codrivercustom.util.ToastUtils;
import com.baidu.che.codrivercustom.widget.FuncButton;
import com.baidu.che.codriversdk.manager.CdTTSPlayerManager;

import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.LinearLayout;

/**
 * TTS语音播报管理类
 */

public class TTSManagerActivity extends BaseActivity {

    private EditText editText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        editText = new EditText(mContext);
        editText.setTextSize(20);
        editText.setText("百度车联网");
        LinearLayout.LayoutParams lp =
                new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        mLLContent.addView(editText, lp);

        addFunctionBtn(new FuncButton(mContext, "播报指定文本(无监听)", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String text = editText.getText().toString().trim();
                if (TextUtils.isEmpty(text)) {
                    ToastUtils.show("播报内容不能为空");
                } else {
                    CdTTSPlayerManager.getInstance().play(text);
                }
            }
        }), new FuncButton(mContext, "播报指定文本(带监听)", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String text = editText.getText().toString().trim();
                if (TextUtils.isEmpty(text)) {
                    ToastUtils.show("播放内容不能为空");
                } else {
                    //播放指定文本的TTS语音，若需要监听播放状态，需要先设
                    // 置setTTSPlayStatusListener(TSPlayStatusListener listener)
                    //utteranceId 文本对应的ID，方便在回调时区分是哪一条文本的状态回调
                    CdTTSPlayerManager.getInstance().playWithUtteranceId(text, "TextId-01");
                }
            }
        }), new FuncButton(mContext, "播报指定文本(显示对话流界面-无监听)", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String text = editText.getText().toString().trim();
                if (TextUtils.isEmpty(text)) {
                    ToastUtils.show("播放内容不能为空");
                } else {
                    CdTTSPlayerManager.getInstance().playAndShow(text);
                }
            }
        }), new FuncButton(mContext, "播报指定文本(显示对话流界面-带监听)", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String text = editText.getText().toString().trim();
                if (TextUtils.isEmpty(text)) {
                    ToastUtils.show("播放内容不能为空");
                } else {
                    //PlayAndShowListener  TTS播报时的对话流界面状态监听
                    CdTTSPlayerManager.getInstance().playAndShow(text, new CdTTSPlayerManager.PlayAndShowListener() {
                        @Override
                        public void onVrDismiss() {
                            //对话流界面关闭的时候,回调到该方法,根据需要执行相应的动作
                        }
                    });
                }
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "停止播报", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdTTSPlayerManager.getInstance().stop();
                ToastUtils.show("停止播报");
            }
        }), new FuncButton(mContext, "设置发音类型", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //目前支持的发音类型,根据需要填写
                //标准女生 NORMAL_FEMALE
                //情感女生 EMOTION_FEMALE,
                //标准男生 NORMAL_MALE
                //情感男生 EMOTION_MALE,
                //度小娇 EMOTION_DUXIAOJIAO_FEMALE,
                CdTTSPlayerManager.getInstance().
                        switchSpeak(CdTTSPlayerManager.SpeechType.NORMAL_FEMALE);
            }
        }));

        addFunctionBtn(new FuncButton(mContext,
                "设置所有TTS播报(DuerOS和使用者)的监听回调",
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        CdTTSPlayerManager.getInstance().setTTSPlayerListener(mTtsPlayerListener);
                        ToastUtils.show("设置所有TTS播报(DuerOS和使用者)的监听回调");
                    }
                }));

        addFunctionBtn(new FuncButton(mContext,
                "设置当前应用(SDK)通过play或playWithUtteranceId接口播报TTS的监听回调",
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        CdTTSPlayerManager.getInstance().setTTSPlayStatusListener(mTtsPlayStatusListener);
                        ToastUtils.show("设置当前应用(SDK)通过play或playWithUtteranceId接口播报TTS的监听回调");
                    }
                }));

        addFunctionBtn(new FuncButton(mContext, "设置普通多轮场景间的延迟时间(输入框输入时间)", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String text = editText.getText().toString().trim();
                long time;
                try {
                    time = Long.parseLong(text);
                } catch (NumberFormatException e) {
                    time = -1;
                    e.printStackTrace();
                }
                if (time == -1) {
                    ToastUtils.show("设置的延迟时间不合法");
                } else {
                    Log.d("TTSManagerActivity", "time = " + time);
                    CdTTSPlayerManager.getInstance().setVrTryAgainDelayTime(time);
                }
            }
        }));
    }

    private CdTTSPlayerManager.TTSPlayerListener mTtsPlayerListener =
            new CdTTSPlayerManager.TTSPlayerListener() {
                @Override
                public void onSpeechStart(String s) {
                    ToastUtils.show("开始播报" + s);
                }

                @Override
                public void onSpeechFinish(String s) {
                    ToastUtils.show("结束播报" + s);
                }

                @Override
                public void onError(String s) {
                    ToastUtils.show("播报错误" + s);
                }
            };

    private CdTTSPlayerManager.TTSPlayStatusListener mTtsPlayStatusListener =
            new CdTTSPlayerManager.TTSPlayStatusListener() {

                @Override
                public void onPlayStart(String utteranceId) {
                    ToastUtils.show("开始播报:" + utteranceId);
                }

                @Override
                public void onPlayFinish(String utteranceId) {
                    ToastUtils.show("播报完成:" + utteranceId);
                }

                @Override
                public void onPlayInterrupt(String utteranceId) {
                    ToastUtils.show("播报中断:" + utteranceId);
                }

                @Override
                public void onPlayBusy(String utteranceId) {
                    ToastUtils.show("当前播报忙:" + utteranceId);
                }

                @Override
                public void onPlayError(String utteranceId) {
                    ToastUtils.show("播报错误:" + utteranceId);
                }
            };

}
