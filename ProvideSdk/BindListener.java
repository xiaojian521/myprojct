package com.android.xj.provide;

public interface BindListener {
    void onServiceConnected();

    void onBindDied();

    void onServiceDisconnected();
}
