package com.android.xj.provide;

/**
 * 外部继承调用
 */
public interface BindListener {
    void onServiceConnected();

    void onBindDied();

    void onServiceDisconnected();
}
