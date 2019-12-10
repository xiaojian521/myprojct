package com.android.xj.provide;

import android.annotation.SuppressLint;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.os.Build.VERSION;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

public class ExternalControl {
    private static final String TAG = "ExternalControl";
    public static final int CODE_SUCCESS = 2000;
    public static final int CODE_NOT_CONNECTED = 2001;
    public static final int CODE_REMOTE_EXCEPTION = 2002;
    public static final int CODE_SERVICE_ERROR = 2003;
    private boolean bServiceConnected;
    private Messenger mSendMessenger;
    private Messenger mReceiveMessenger;
    private List<BindListener> mBindListeners;
    private List<MediaChangeListener> mMediaListeners;
    private List<PlayStateListener> mPlayListeners;
    private List<IAppView> mAppViewListeners;
    @SuppressLint({"HandlerLeak"})
    private Handler mHandler;
    private ServiceConnection mServiceConnection;

    private ExternalControl() {
        this.mHandler = new Handler() {
            public void handleMessage(Message msg) {
                if (msg.what != 2006) {
                    LogUtils.d("ExternalControl", "handleMessage: " + msg.what);
                }

                Bundle data = msg.getData();
                boolean expire_state;
                MediaInfo media;
                switch(msg.what) {
                case 2003:
                    if (data != null) {
                        media = MediaInfoUtil.parseMediaInfo(data);
                        ExternalControl.this.notifyMediaChange(media);
                    }

                    ExternalControl.this.notifyPlayStart();
                    break;
                case 2004:
                    ExternalControl.this.notifyPlayPause();
                    break;
                case 2005:
                    ExternalControl.this.notifyPlayStop();
                    break;
                case 2006:
                    long current = 0L;
                    long total = 0L;
                    String type = null;
                    if (data != null) {
                        current = data.getLong("media_progress_current");
                        total = data.getLong("media_progress_total");
                        type = data.getString("media_type");
                    }

                    ExternalControl.getInstance().notifyProgress(type, current, total);
                    break;
                case 2009:
                    if (data != null) {
                        boolean isFavored = data.getBoolean("media_is_favored");
                        ExternalControl.this.notifyMediaFavorChange(isFavored);
                    }
                    break;
                case 2010:
                    if (data != null) {
                        media = MediaInfoUtil.parseMediaInfo(data);
                        ExternalControl.this.notifyMediaChange(media);
                    }
                    break;
                case 2014:
                    if (data != null) {
                        int mode = data.getInt("media_playmode");
                        ExternalControl.this.notifyPlayModeChange(mode);
                    }
                    break;
                case 2015:
                    ExternalControl.this.notifyPlayListChange();
                    break;
                case 3003:
                    ExternalControl.this.showLoading();
                    break;
                case 3004:
                    ExternalControl.this.hideLoading();
                    break;
                case 3005:
                    if (data != null) {
                        expire_state = data.getBoolean("wx_open_close");
                        boolean isLoginFlag = data.getBoolean("login_state_flag");
                        ExternalControl.this.isWxOpenCloseFlag(expire_state, isLoginFlag);
                    }
                    break;
                case 3006:
                    if (data != null) {
                        expire_state = data.getBoolean("login_state_flag");
                        ExternalControl.this.setLoginState(expire_state);
                    }
                    break;
                case 3007:
                    if (data != null) {
                        expire_state = data.getBoolean("expire_state_flag");
                        ExternalControl.this.setExpireState(expire_state);
                    }
                }

            }
        };
        this.mServiceConnection = new ServiceConnection() {
            public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
                LogUtils.d("ExternalControl", "onServiceConnected");
                ExternalControl.this.bServiceConnected = true;
                ExternalControl.this.mSendMessenger = new Messenger(iBinder);
                ExternalControl.this.mReceiveMessenger = new Messenger(ExternalControl.this.mHandler);
                ExternalControl.this.addMessenger();
                ExternalControl.this.notifyServiceConnected();
            }

            public void onServiceDisconnected(ComponentName componentName) {
                LogUtils.d("ExternalControl", "onServiceDisconnected");
                ExternalControl.this.bServiceConnected = false;
                ExternalControl.this.mSendMessenger = null;
                ExternalControl.this.mReceiveMessenger = null;
                ExternalControl.this.notifyServiceDisconnected();
            }

            public void onBindingDied(ComponentName name) {
                LogUtils.d("ExternalControl", "onBindingDied name: " + name);
                ExternalControl.this.notifyBindDied();
            }
        };
        this.mBindListeners = new LinkedList();
        this.mMediaListeners = new LinkedList();
        this.mPlayListeners = new LinkedList();
        this.mAppViewListeners = new LinkedList();
    }

    public static ExternalControl getInstance() {
        return ExternalControl.Holder.sInstance;
    }

    public void addIAppViewListener(IAppView listener) {
        LogUtils.d("ExternalControl", "addAppViewListener: " + listener);
        if (listener != null) {
            synchronized(this.mAppViewListeners) {
                if (!this.mAppViewListeners.contains(listener)) {
                    this.mAppViewListeners.add(listener);
                }

            }
        }
    }

    public void removeIAppViewListener(IAppView listener) {
        LogUtils.d("ExternalControl", "removeIAppViewListener: " + listener);
        if (listener != null) {
            synchronized(this.mAppViewListeners) {
                this.mAppViewListeners.remove(listener);
            }
        }
    }

    private void showLoading() {
        LogUtils.d("ExternalControl", "  showLoading  ");
        synchronized(this.mAppViewListeners) {
            Iterator var2 = this.mAppViewListeners.iterator();

            while(var2.hasNext()) {
                IAppView listener = (IAppView)var2.next();
                listener.beginSearch();
            }

        }
    }

    private void hideLoading() {
        LogUtils.d("ExternalControl", "  hideLoading  ");
        synchronized(this.mAppViewListeners) {
            Iterator var2 = this.mAppViewListeners.iterator();

            while(var2.hasNext()) {
                IAppView listener = (IAppView)var2.next();
                listener.finishSearch();
            }

        }
    }

    private void isWxOpenCloseFlag(boolean flag, boolean isLogin) {
        LogUtils.d("ExternalControl", "  isOpenCloseFlag  " + flag + " isLogin = " + isLogin);
        synchronized(this.mAppViewListeners) {
            Iterator var4 = this.mAppViewListeners.iterator();

            while(var4.hasNext()) {
                IAppView listener = (IAppView)var4.next();
                listener.onWXBindActivityOpenOrClose(flag, isLogin);
            }

        }
    }

    private void setLoginState(boolean flag) {
        LogUtils.d("ExternalControl", "  setLoginState  " + flag);
        synchronized(this.mAppViewListeners) {
            Iterator var3 = this.mAppViewListeners.iterator();

            while(var3.hasNext()) {
                IAppView listener = (IAppView)var3.next();
                listener.setLoginUI(flag);
            }

        }
    }

    private void setExpireState(boolean flag) {
        LogUtils.d("ExternalControl", "  setLoginState  " + flag);
        synchronized(this.mAppViewListeners) {
            Iterator var3 = this.mAppViewListeners.iterator();

            while(var3.hasNext()) {
                IAppView listener = (IAppView)var3.next();
                listener.onExpireChanged(flag);
            }

        }
    }

    public boolean startPlayActivity(Context context) {
        LogUtils.d("ExternalControl", "startPlayActivity");

        try {
            ComponentName comp = new ComponentName("com.tencent.wecar", "com.tencent.wecarflow.MainActivity");
            Intent intent = new Intent();
            intent.setComponent(comp);
            intent.setFlags(268435456);
            context.startActivity(intent);
            return true;
        } catch (ActivityNotFoundException var4) {
            LogUtils.d("ExternalControl", "应用未安装");
            return false;
        }
    }

    public boolean semanticSearch(Context context, String from, String semantic) {
        LogUtils.d("ExternalControl", "semanticSearch from: " + from + ", semantic: " + semantic);

        try {
            ComponentName comp = new ComponentName("com.tencent.wecar", "com.tencent.wecarflow.MainActivity");
            Intent intent = new Intent();
            intent.setComponent(comp);
            intent.putExtra("from", from);
            intent.putExtra("where", 1);
            intent.putExtra("taskId", 0L);
            intent.putExtra("semantic", semantic);
            intent.setFlags(268435456);
            context.startActivity(intent);
            return true;
        } catch (ActivityNotFoundException var6) {
            LogUtils.d("ExternalControl", "应用未安装");
            return false;
        }
    }

    public int semanticSearchInService(String from, String semantic) {
        LogUtils.d("ExternalControl", "semanticSearchInService");
        Message msg = Message.obtain();
        msg.what = 1012;
        Bundle data = new Bundle();
        data.putInt("where", 1);
        data.putLong("taskId", 0L);
        data.putString("from", from);
        data.putString("semantic", semantic);
        msg.setData(data);
        return this.sendMessage(msg);
    }

    public boolean startPlay(Context context, String from, String semantic) {
        LogUtils.d("ExternalControl", "startPlay from: " + from + ", semantic: " + semantic);

        try {
            ComponentName comp = new ComponentName("com.tencent.wecar", "com.tencent.wecarflow.MainActivity");
            Intent intent = new Intent();
            intent.setComponent(comp);
            intent.putExtra("from", from);
            intent.putExtra("where", 4);
            intent.putExtra("taskId", 0);
            intent.putExtra("semantic", semantic);
            intent.setFlags(268435456);
            context.startActivity(intent);
            return true;
        } catch (ActivityNotFoundException var6) {
            LogUtils.d("ExternalControl", "应用未安装");
            return false;
        }
    }

    public boolean playFavor(Context context, String from, String semantic) {
        LogUtils.d("ExternalControl", "playFavor from: " + from + ", semantic: " + semantic);

        try {
            ComponentName comp = new ComponentName("com.tencent.wecar", "com.tencent.wecarflow.MainActivity");
            Intent intent = new Intent();
            intent.setComponent(comp);
            intent.putExtra("from", from);
            intent.putExtra("where", 3);
            intent.putExtra("taskId", 0);
            intent.putExtra("semantic", semantic);
            intent.setFlags(268435456);
            context.startActivity(intent);
            return true;
        } catch (ActivityNotFoundException var6) {
            LogUtils.d("ExternalControl", "应用未安装");
            return false;
        }
    }

    public boolean showFavor(Context context, String from, String semantic) {
        LogUtils.d("ExternalControl", "showFavor from: " + from + ", semantic: " + semantic);

        try {
            ComponentName comp = new ComponentName("com.tencent.wecar", "com.tencent.wecarflow.MainActivity");
            Intent intent = new Intent();
            intent.setComponent(comp);
            intent.putExtra("from", from);
            intent.putExtra("where", 2);
            intent.putExtra("taskId", 0);
            intent.putExtra("semantic", semantic);
            intent.setFlags(268435456);
            context.startActivity(intent);
            return true;
        } catch (ActivityNotFoundException var6) {
            LogUtils.d("ExternalControl", "应用未安装");
            return false;
        }
    }

    public void bindPlayService(Context context) {
        LogUtils.d("ExternalControl", "bindPlayService");
        ComponentName componentName = new ComponentName("com.tencent.wecarflow", "com.tencent.wecarflow.launcherwidget.UpdateWidgetService");
        Intent start = new Intent();
        start.setComponent(componentName);
        start.setAction("com.tencent.wecarflow.service_init");
        if (VERSION.SDK_INT >= 26) {
            context.startForegroundService(start);
        } else {
            context.startService(start);
        }

        start.setComponent(componentName);
        context.bindService(start, this.mServiceConnection, 1);
    }

    public int closeApp() {
        LogUtils.d("ExternalControl", "closeApp");
        Message msg = Message.obtain();
        msg.what = 1008;
        return this.sendMessage(msg);
    }

    public void unbindPlayService(Context context) {
        LogUtils.d("ExternalControl", "unbindPlayService");
        if (this.bServiceConnected) {
            this.removeMessenger();
            context.unbindService(this.mServiceConnection);
            this.bServiceConnected = false;
            this.mSendMessenger = null;
            this.mReceiveMessenger = null;
        }

    }

    public int addFavor() {
        LogUtils.d("ExternalControl", "addFavor");
        Message msg = Message.obtain();
        msg.what = 1009;
        return this.sendMessage(msg);
    }

    public int cancelFavor() {
        LogUtils.d("ExternalControl", "cancelFavor");
        Message msg = Message.obtain();
        msg.what = 1010;
        return this.sendMessage(msg);
    }

    public int setPlayMode(int mode) {
        LogUtils.d("ExternalControl", "setPlayMode");
        Message msg = Message.obtain();
        msg.what = 1011;
        msg.arg1 = mode;
        return this.sendMessage(msg);
    }

    public int doPre() {
        LogUtils.d("ExternalControl", "doPre");
        Message msg = Message.obtain();
        msg.what = 1002;
        return this.sendMessage(msg);
    }

    public int doNext() {
        LogUtils.d("ExternalControl", "doNext");
        Message msg = Message.obtain();
        msg.what = 1001;
        return this.sendMessage(msg);
    }

    public int doRecommend() {
        LogUtils.d("ExternalControl", "doRecommend");
        Message msg = Message.obtain();
        msg.what = 1013;
        return this.sendMessage(msg);
    }

    public int doPause() {
        LogUtils.d("ExternalControl", "doPause");
        Message msg = Message.obtain();
        msg.what = 1004;
        return this.sendMessage(msg);
    }

    public int doPlay() {
        LogUtils.d("ExternalControl", "doPlay");
        Message msg = Message.obtain();
        msg.what = 1003;
        return this.sendMessage(msg);
    }

    public int doStop() {
        LogUtils.d("ExternalControl", "doStop");
        Message msg = Message.obtain();
        msg.what = 1005;
        return this.sendMessage(msg);
    }

    public int queryPlaying(QueryCallback<Boolean> callback) {
        LogUtils.d("ExternalControl", "queryPlaying: " + callback);
        Message msg = Message.obtain();
        msg.what = 1006;
        msg.replyTo = new Messenger(new CallbackHandler(callback));
        return this.sendMessage(msg);
    }

    public int queryCurrent(QueryCallback<MediaInfo> callback) {
        LogUtils.d("ExternalControl", "queryCurrent: " + callback);
        Message msg = Message.obtain();
        msg.what = 1007;
        msg.replyTo = new Messenger(new CallbackHandler(callback));
        return this.sendMessage(msg);
    }

    public int getCurrentMedia(QueryCallback<MediaInfo> callback) {
        LogUtils.d("ExternalControl", "getCurrentMedia: " + callback);
        Message msg = Message.obtain();
        msg.what = 1014;
        msg.replyTo = new Messenger(new CallbackHandler(callback));
        return this.sendMessage(msg);
    }

    public int getCurrentIndex(QueryCallback<Integer> callback) {
        Message msg = new Message();
        msg.what = 1015;
        msg.replyTo = new Messenger(new CallbackHandler(callback));
        return this.sendMessage(msg);
    }

    public int doPlay(int index) {
        LogUtils.d("ExternalControl", "doPlay index");
        Message msg = Message.obtain();
        msg.what = 1018;
        msg.arg1 = index;
        return this.sendMessage(msg);
    }

    public int getCurrentList(QueryCallback<List<MediaInfo>> callback) {
        LogUtils.d("ExternalControl", "getCurrentList");
        Message msg = Message.obtain();
        msg.what = 1017;
        msg.replyTo = new Messenger(new CallbackHandler(callback));
        return this.sendMessage(msg);
    }

    public int isFirst(QueryCallback<Boolean> callback) {
        Message msg = Message.obtain();
        msg.what = 1019;
        msg.replyTo = new Messenger(new CallbackHandler(callback));
        return this.sendMessage(msg);
    }

    public int isLast(QueryCallback<Boolean> callback) {
        Message msg = Message.obtain();
        msg.what = 1020;
        msg.replyTo = new Messenger(new CallbackHandler(callback));
        return this.sendMessage(msg);
    }

    public void addBindListener(BindListener listener) {
        LogUtils.d("ExternalControl", "addBindListener: " + listener);
        if (listener != null) {
            synchronized(this.mBindListeners) {
                if (!this.mBindListeners.contains(listener)) {
                    this.mBindListeners.add(listener);
                }

            }
        }
    }

    public void removeBindListener(BindListener listener) {
        LogUtils.d("ExternalControl", "removeBindListener: " + listener);
        if (listener != null) {
            synchronized(this.mBindListeners) {
                this.mBindListeners.remove(listener);
            }
        }
    }

    public void addMediaChangeListener(MediaChangeListener listener) {
        LogUtils.d("ExternalControl", "addMediaChangeListener: " + listener);
        if (listener != null) {
            synchronized(this.mMediaListeners) {
                if (!this.mMediaListeners.contains(listener)) {
                    this.mMediaListeners.add(listener);
                }

            }
        }
    }

    public void removeMediaChangeListener(MediaChangeListener listener) {
        LogUtils.d("ExternalControl", "removeMediaChangeListener: " + listener);
        if (listener != null) {
            synchronized(this.mMediaListeners) {
                this.mMediaListeners.remove(listener);
            }
        }
    }

    public void addPlayStateListener(PlayStateListener listener) {
        LogUtils.d("ExternalControl", "addPlayStateListener: " + listener);
        if (listener != null) {
            synchronized(this.mPlayListeners) {
                if (!this.mPlayListeners.contains(listener)) {
                    this.mPlayListeners.add(listener);
                }

            }
        }
    }

    public void removePlayStateListener(PlayStateListener listener) {
        LogUtils.d("ExternalControl", "removePlayStateListener: " + listener);
        if (listener != null) {
            synchronized(this.mPlayListeners) {
                this.mPlayListeners.remove(listener);
            }
        }
    }

    private void notifyMediaChange(MediaInfo info) {
        LogUtils.d("ExternalControl", "notifyMediaChange info: " + info.getMediaName());
        synchronized(this.mMediaListeners) {
            Iterator var3 = this.mMediaListeners.iterator();

            while(var3.hasNext()) {
                MediaChangeListener listener = (MediaChangeListener)var3.next();
                listener.onMediaChange(info);
            }

        }
    }

    private void notifyMediaFavorChange(boolean isFavored) {
        LogUtils.d("ExternalControl", "notifyMediaFavorChange isFavored: " + isFavored);
        synchronized(this.mMediaListeners) {
            Iterator var3 = this.mMediaListeners.iterator();

            while(var3.hasNext()) {
                MediaChangeListener listener = (MediaChangeListener)var3.next();
                listener.onFavorChange(isFavored);
            }

        }
    }

    private void notifyPlayModeChange(int mode) {
        synchronized(this.mMediaListeners) {
            Iterator var3 = this.mMediaListeners.iterator();

            while(var3.hasNext()) {
                MediaChangeListener listener = (MediaChangeListener)var3.next();
                listener.onModeChange(mode);
            }

        }
    }

    private void notifyPlayListChange() {
        synchronized(this.mMediaListeners) {
            Iterator var2 = this.mMediaListeners.iterator();

            while(var2.hasNext()) {
                MediaChangeListener listener = (MediaChangeListener)var2.next();
                listener.onPlayListChange();
            }

        }
    }

    private void notifyPlayStart() {
        LogUtils.d("ExternalControl", "notifyPlayStart");
        synchronized(this.mPlayListeners) {
            Iterator var2 = this.mPlayListeners.iterator();

            while(var2.hasNext()) {
                PlayStateListener listener = (PlayStateListener)var2.next();
                listener.onStart();
            }

        }
    }

    private void notifyPlayPause() {
        LogUtils.d("ExternalControl", "notifyPlayPause");
        synchronized(this.mPlayListeners) {
            Iterator var2 = this.mPlayListeners.iterator();

            while(var2.hasNext()) {
                PlayStateListener listener = (PlayStateListener)var2.next();
                listener.onPause();
            }

        }
    }

    private void notifyPlayStop() {
        LogUtils.d("ExternalControl", "notifyPlayStop");
        synchronized(this.mPlayListeners) {
            Iterator var2 = this.mPlayListeners.iterator();

            while(var2.hasNext()) {
                PlayStateListener listener = (PlayStateListener)var2.next();
                listener.onStop();
            }

        }
    }

    void notifyProgress(String type, long current, long total) {
        synchronized(this.mPlayListeners) {
            Iterator var7 = this.mPlayListeners.iterator();

            while(var7.hasNext()) {
                PlayStateListener listener = (PlayStateListener)var7.next();
                listener.onProgress(type, current, total);
            }

        }
    }

    private void notifyServiceConnected() {
        LogUtils.d("ExternalControl", "notifyServiceConnected");
        synchronized(this.mBindListeners) {
            Iterator var2 = this.mBindListeners.iterator();

            while(var2.hasNext()) {
                BindListener listener = (BindListener)var2.next();
                listener.onServiceConnected();
            }

        }
    }

    private void notifyServiceDisconnected() {
        LogUtils.d("ExternalControl", "notifyServiceDisconnected");
        synchronized(this.mBindListeners) {
            Iterator var2 = this.mBindListeners.iterator();

            while(var2.hasNext()) {
                BindListener listener = (BindListener)var2.next();
                listener.onServiceDisconnected();
            }

        }
    }

    private void notifyBindDied() {
        LogUtils.d("ExternalControl", "notifyBindDied: ");
        synchronized(this.mBindListeners) {
            Iterator var2 = this.mBindListeners.iterator();

            while(var2.hasNext()) {
                BindListener listener = (BindListener)var2.next();
                listener.onBindDied();
            }

        }
    }

    private void semanticSearch() {
    }

    private void addMessenger() {
        LogUtils.d("ExternalControl", "addMessenger");
        Message msg = new Message();
        msg.what = 3001;
        msg.replyTo = this.mReceiveMessenger;
        this.sendMessage(msg);
    }

    private void removeMessenger() {
        LogUtils.d("ExternalControl", "removeMessenger");
        Message msg = new Message();
        msg.what = 3002;
        msg.replyTo = this.mReceiveMessenger;
        this.sendMessage(msg);
    }

    public boolean isServiceConnected() {
        return this.bServiceConnected;
    }

    private int sendMessage(Message msg) {
        LogUtils.d("ExternalControl", "sendMessage type: " + msg.what + ", bServiceConnected: " + this.bServiceConnected);
        if (this.bServiceConnected) {
            try {
                this.mSendMessenger.send(msg);
                LogUtils.d("ExternalControl", "sendMessage success: ");
                return 2000;
            } catch (RemoteException var3) {
                LogUtils.d("ExternalControl", "sendMessage failed: " + var3.getMessage());
                var3.printStackTrace();
                return 2002;
            }
        } else {
            return 2001;
        }
    }

    private static class Holder {
        private static ExternalControl sInstance = new ExternalControl();

        private Holder() {
        }
    }
}
