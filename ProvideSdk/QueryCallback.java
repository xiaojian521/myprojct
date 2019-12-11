package com.android.xj.provide;

/**
 * 请求模板基类
 * @param <T>
 */
public interface QueryCallback<T> {
    void onError(int var1);

    void onSuccess(T var1);
}
