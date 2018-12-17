/*
 * Copyright (C) 2017 Baidu, Inc. All Rights Reserved.
 */
package com.baidu.che.codrivercustom;

import com.baidu.che.codriversdk.LogUtil;
import com.baidu.che.codriversdk.manager.CdConfigManager;

import android.app.Application;
import android.os.Handler;
import android.os.Looper;

public class CoDriverCustomApp extends Application {

    private static final String TAG = "CoDriverCustomApp";
    private static CoDriverCustomApp mInstance = null;
    private static Handler mHandler = new Handler(Looper.getMainLooper());

    public static void runUITask(Runnable run) {
        mHandler.post(run);
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mInstance = this;
        LogUtil.d(TAG, "onCreate()");
        //sdk执行初始化
        CdConfigManager.getInstance().
                initialize(getApplicationContext(), new CoDriverRunnable());
    }

    public static CoDriverCustomApp getInstance() {
        return mInstance;
    }

}
