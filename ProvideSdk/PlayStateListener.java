package com.android.xj.provide;

/**
 * 播放状态变更通知
 */
public interface PlayStateListener {
    void onStart();

    void onPause();

    void onStop();

    void onProgress(String var1, long var2, long var4);
}
