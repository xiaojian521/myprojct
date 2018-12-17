package com.baidu.che.codrivercustom.receiver;

import com.baidu.che.codrivercustom.util.LogUtil;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class BootReceiver extends BroadcastReceiver {

    private static final String TAG = "BootReceiver";

    private static final String BOOT_COMPLETED = "android.intent.action.BOOT_COMPLETED";
    private static final String CODRIVER_CUSTOM_START = "com.baidu.che.codrivercustom.START";

    @Override
    public void onReceive(Context context, Intent intent) {
        LogUtil.d(TAG, "onReceive");
        if (intent == null) {
            return;
        }

        LogUtil.d(TAG, "BootReceiver：" + intent.getAction());
        if (intent.getAction().equals(BOOT_COMPLETED)) {
            // 发天气广播测试
            Intent iWeather = new Intent();
            iWeather.setAction("com.hkmc.intent.action.request_weather_update");
            context.sendBroadcast(iWeather);
            LogUtil.d(TAG, "Action: " + BOOT_COMPLETED);
        } else if (intent.getAction().equals(CODRIVER_CUSTOM_START)) {
            LogUtil.d(TAG, "Action: " + CODRIVER_CUSTOM_START);
        }
    }

}
