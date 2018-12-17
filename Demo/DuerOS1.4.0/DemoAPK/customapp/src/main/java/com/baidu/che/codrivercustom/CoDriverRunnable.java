package com.baidu.che.codrivercustom;

import com.baidu.che.codrivercustom.util.LogUtil;
import com.baidu.che.codriversdk.InitListener;
import com.baidu.che.codriversdk.manager.CdAsrManager;
import com.baidu.che.codriversdk.manager.CdConfigManager;

public class CoDriverRunnable implements InitListener {

    private static final String TAG = "CoDriverCustomApp";

    public CoDriverRunnable() {
    }

    @Override
    public void onConnectedToRemote() {
        //sdk初始化成功
        LogUtil.d(TAG, "onConnectedToRemote");

        /**
         * 与DuerOS连接成功：可以调用定制化接口
         * 这里初始化各种模块，进行功能定制
         * 下面仅仅加入了部分功能, 其他功能的接入示例,请参考ui包下Activity的具体实现
         */

        //For example ===========================================

        /**
         * 打开sdk日志
         */
        CdConfigManager.getInstance().openLog(true);

        /**
         * 设置语音流程相关回调
         */
        CdAsrManager.getInstance().setAsrTool(new CdAsrManager.AsrTool() {
            @Override
            public void onVrDialogShow() {
                LogUtil.d(TAG, "显示对话流界面");
            }

            @Override
            public void onVrDialogDismiss() {
                LogUtil.d(TAG, "退出对话流界面");
            }

            @Override
            public void onWakeUp() {

            }
        });

    }

    @Override
    public void onDisconnectedToRemote() {
        // 与DuerOS连接断开：可以做一些清理工作
        LogUtil.d(TAG, "onDisconnectedToRemote");
    }

}
