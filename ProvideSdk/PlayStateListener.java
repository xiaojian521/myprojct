package com.android.xj.provide;

public interface PlayStateListener {
    void onStart();

    void onPause();

    void onStop();

    void onProgress(String var1, long var2, long var4);
}
