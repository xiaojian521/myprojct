/*
 * Copyright (C) 2017 Baidu, Inc. All Rights Reserved.
 */
package com.baidu.che.codrivercustom.ui;

import com.baidu.che.codrivercustom.util.LogUtil;
import com.baidu.che.codrivercustom.util.ToastUtils;
import com.baidu.che.codrivercustom.widget.FuncButton;
import com.baidu.che.codriversdk.manager.CdCarInfoQueryManager;
import com.baidu.che.codriversdk.manager.CdConfigManager;
import com.baidu.che.codriversdk.manager.CdIntelligentModeManager;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends BaseActivity {
    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addFunctionBtn(new FuncButton(mContext, "配置管理", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startDemo("配置管理", ConfigManagerActivity.class);
            }
        }), new FuncButton(mContext, "系统控制", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startDemo("车辆控制", SystemManagerActivity.class);
            }
        }), new FuncButton(mContext, "TTS语音播报管理", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startDemo("TTS语音播报管理", TTSManagerActivity.class);
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "多媒体功能管理", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startDemo("多媒体功能管理", MediaManagerActivity.class);
            }
        }), new FuncButton(mContext, "蓝牙电话功能", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startDemo("蓝牙电话功能", BlueToothCallManagerActivity.class);
            }
        }), new FuncButton(mContext, "导航功能", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startDemo("导航功能", NaviActivity.class);
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "音乐功能", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startDemo("音乐功能", MusicActivity.class);
            }
        }), new FuncButton(mContext, "语音识别", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startDemo("语音识别", AsrManagerActivity.class);
            }
        }), new FuncButton(mContext, "电子手册", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdCarInfoQueryManager.getInstance().setQueryCarInfoTool(new CdCarInfoQueryManager.QueryCarInfoTool() {
                    @Override
                    public boolean answerContent(String feature, String extra) {
                        LogUtil.d(TAG, "answerContent() feature = " + feature + ", extra = " + extra);
                        ToastUtils.show("answerContent() feature = " + feature + ", extra = " + extra);
                        return false;
                    }

                    @Override
                    public boolean displayInfo(String feature, String extra) {
                        LogUtil.d(TAG, "displayInfo() feature = " + feature + ", extra = " + extra);
                        ToastUtils.show("displayInfo() feature = " + feature + ", extra = " + extra);
                        return false;
                    }
                });
                ToastUtils.show("设置车辆信息查询监听");
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "日程管理功能", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startDemo("日程管理功能", ScheduleManagerActivity.class);
            }
        }), new FuncButton(mContext, "有声节目功能", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startDemo("有声节目功能", PrivateRadioActivity.class);
            }
        }), new FuncButton(mContext, "智能化命令", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdIntelligentModeManager.getInstance().setIntelligentModeListener(new CdIntelligentModeManager.IntelligentModeListener() {
                    @Override
                    public boolean onIntelligentMode(String mode, String extra) {
                        LogUtil.d(TAG, "onIntelligentMode() feature = " + mode + ", extra = " + extra);
                        ToastUtils.show("onIntelligentMode() feature = " + mode + ", extra = " + extra);
                        return false;
                    }
                });
            }
        }));
    }

    private void startDemo(String name, Class<? extends Activity> activity) {
        try {
            if (!CdConfigManager.getInstance().isInitSuccess()) {
                ToastUtils.show("sdk初始化未成功, 请安装DuerOS.apk");
                return;
            }
            Intent intent = new Intent(mContext, activity);
            intent.putExtra("title", name);
            startActivity(intent);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
