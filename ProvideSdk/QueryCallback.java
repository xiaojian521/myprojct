package com.android.xj.provide;

public interface QueryCallback<T> {
    void onError(int var1);

    void onSuccess(T var1);
}
