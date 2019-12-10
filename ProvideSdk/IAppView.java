package com.android.xj.provide;

import android.content.Context;
import android.content.Intent;

public interface IAppView {
    Context getContext();

    Intent getLaunchIntent();

    void beginSearch();

    void finishSearch();

    void setLoginUI(boolean var1);

    void showFavor(long var1, boolean var3);

    void showSearch(boolean var1);

    void showQQLogin();

    void showBindingList();

    void showBinding(int var1);

    void clear();

    void showLogin(long var1);

    void switchToPlayer();

    void switchToFinder();

    void onExpireChanged(boolean var1);

    void onWXBindActivityOpenOrClose(boolean var1, boolean var2);
}
