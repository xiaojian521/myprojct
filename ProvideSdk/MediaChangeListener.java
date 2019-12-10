package com.android.xj.provide;

public interface MediaChangeListener {
    void onMediaChange(MediaInfo var1);

    void onFavorChange(boolean var1);

    void onModeChange(int var1);

    void onPlayListChange();
}
