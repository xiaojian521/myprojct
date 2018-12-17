package com.baidu.che.codrivercustom.ui;

import com.baidu.che.codrivercustom.util.ToastUtils;
import com.baidu.che.codrivercustom.widget.FuncButton;
import com.baidu.che.codriversdk.manager.CdAsrManager;
import com.baidu.che.codriversdk.manager.CdConfigManager;

import android.os.Bundle;
import android.view.View;

/**
 * 配置管理
 */

public class ConfigManagerActivity extends BaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addFunctionBtn(new FuncButton(mContext, "休眠DuerOS", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdConfigManager.getInstance().notifySystemSleep();
                ToastUtils.show("休眠DuerOS");
            }
        }), new FuncButton(mContext, "唤醒DuerOS", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdConfigManager.getInstance().notifySystemWakeUp();
                ToastUtils.show("唤醒DuerOS");
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "启动DuerOS", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdAsrManager.getInstance().openDialog("小度来了");
                ToastUtils.show("打开语音对话流");
            }
        }), new FuncButton(mContext, "关闭DuerOS", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdAsrManager.getInstance().closeDialog();
                ToastUtils.show("关闭语音对话流");
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "打开日志开关", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdConfigManager.getInstance().openLog(true);
                ToastUtils.show("打开日志开关");
            }
        }), new FuncButton(mContext, "关闭日志开关", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdConfigManager.getInstance().openLog(false);
                ToastUtils.show("关闭日志开关");
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "设置语音反馈为不播报", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdConfigManager.getInstance().setCmdFeedbackMode(CdConfigManager.CmdFeedbackMode.NO_TTS);
                ToastUtils.show("设置语音反馈为不播报");
            }
        }), new FuncButton(mContext, "设置语音反馈为全部播报", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdConfigManager.getInstance().setCmdFeedbackMode(CdConfigManager.CmdFeedbackMode.TTS_BOTH);
                ToastUtils.show("设置语音反馈为全部播报");
            }
        }));
        addFunctionBtn(new FuncButton(mContext, "设置语音反馈为执行成功播报", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdConfigManager.getInstance().setCmdFeedbackMode(CdConfigManager.CmdFeedbackMode.TTS_WHEN_EXECUTE);
                ToastUtils.show("设置语音反馈为执行成功播报");
            }
        }), new FuncButton(mContext, "设置语音反馈为执行失败播报", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdConfigManager.getInstance().setCmdFeedbackMode(CdConfigManager.CmdFeedbackMode.TTS_WHEN_NOT_EXECUTE);
                ToastUtils.show("设置语音反馈为执行失败播报");
            }
        }));
//        addTitle(20, "设置对话流界面样式", null);

        // 任意选择一种样式进行设置
//        addFunctionBtn(new FuncButton(mContext, "activity样式", new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                CdConfigManager.getInstance().setDisplayView(CdConfigManager.DisplayView.ACTIVITY);
//                ToastUtils.show("设置为activity样式");
//            }
//        }), new FuncButton(mContext, "dialog样式", new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                CdConfigManager.getInstance().setDisplayView(CdConfigManager.DisplayView.DIALOG);
//                ToastUtils.show("设置为dialog样式");
//            }
//        }));

    }
}
