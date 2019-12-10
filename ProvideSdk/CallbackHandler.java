package com.android.xj.provide;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import java.util.List;

public class CallbackHandler extends Handler {
    private static final String TAG = "CallbackHandler";
    private QueryCallback callback;

    public CallbackHandler(QueryCallback callback) {
        this.callback = callback;
    }

    public void handleMessage(Message msg) {
        LogUtils.d("CallbackHandler", "handleMessage: " + msg.what);
        switch(msg.what) {
        case 2007:
            Bundle bundle = msg.getData();
            if (bundle != null) {
                boolean playing = bundle.getBoolean("media_playing");
                this.callback.onSuccess(playing);
                LogUtils.d("CallbackHandler", "callback onSuccess playing: " + playing);
            } else {
                this.callback.onError(2003);
                LogUtils.d("CallbackHandler", "callback onError");
            }
            break;
        case 2008:
        case 2011:
            Bundle curItem = msg.getData();
            if (curItem != null) {
                MediaInfo media = MediaInfoUtil.parseMediaInfo(curItem);
                this.callback.onSuccess(media);
                LogUtils.d("CallbackHandler", "callback onSuccess media: " + media.getMediaName());
            } else {
                this.callback.onError(2003);
                LogUtils.d("CallbackHandler", "callback onError");
            }
        case 2009:
        case 2010:
        case 2014:
        case 2015:
        default:
            break;
        case 2012:
            Bundle dataIndex = msg.getData();
            if (dataIndex != null) {
                int index = dataIndex.getInt("media_index");
                this.callback.onSuccess(index);
                LogUtils.d("CallbackHandler", "callback onSuccess jsonArray: " + index);
            } else {
                this.callback.onError(2003);
                LogUtils.d("CallbackHandler", "callback onError");
            }
            break;
        case 2013:
            Bundle data = msg.getData();
            if (data != null) {
                String jsonArray = data.getString("media_get_play_list");
                List<MediaInfo> infoList = MediaInfoUtil.parseMediaInfoList(jsonArray);
                this.callback.onSuccess(infoList);
                LogUtils.d("CallbackHandler", "callback onSuccess jsonArray: " + jsonArray);
            } else {
                this.callback.onError(2003);
                LogUtils.d("CallbackHandler", "callback onError");
            }
            break;
        case 2016:
            Bundle firstData = msg.getData();
            if (firstData != null) {
                boolean ret = firstData.getBoolean("is_first");
                this.callback.onSuccess(ret);
                LogUtils.d("CallbackHandler", "callback onSuccess isFirst: " + ret);
            } else {
                this.callback.onError(2003);
            }
            break;
        case 2017:
            Bundle lastData = msg.getData();
            if (lastData != null) {
                boolean ret = lastData.getBoolean("is_last");
                this.callback.onSuccess(ret);
                LogUtils.d("CallbackHandler", "callback onSuccess isFirst: " + ret);
            } else {
                this.callback.onError(2003);
            }
        }

    }
}
