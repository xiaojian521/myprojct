package com.android.xj.provide;

import android.util.Log;

class LogUtils {
    private static final String TAG = "ProvideSdk";

    LogUtils() {
    }

    public static void d(String tag, String msg) {
        Log.d("ProvideSdk_" + tag, msg);
    }

    public static void e(String tag, String msg) {
        Log.e("ProvideSdk_" + tag, msg);
    }

    public static void i(String tag, String msg) {
        Log.i("ProvideSdk_" + tag, msg);
    }

    public static void v(String tag, String msg) {
        Log.v("ProvideSdk_" + tag, msg);
    }

    public static void w(String tag, String msg) {
        Log.w("ProvideSdk_" + tag, msg);
    }
}
