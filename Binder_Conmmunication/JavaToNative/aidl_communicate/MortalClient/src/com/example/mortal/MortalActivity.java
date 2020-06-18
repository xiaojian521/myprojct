package com.example.mortal;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.view.View;
import android.widget.Button;
import android.os.RemoteException;
import android.util.Log;
import android.os.ServiceManager;

import java.io.IOException;
import com.example.mortal.IDeleteFile;
import com.example.mortal.IClientListener;
import com.example.mortal.IServerCallBack;

public class MortalActivity extends Activity implements View.OnClickListener {
    static final private String TAG = "xjtest";

    private Button btn_deleteFile;
    private Button btn_registerListener;

    private IDeleteFile mService;
    private IServerCallBack mCallBack;
    private IClientListener listener;
    

    private static final int CONNECT_SERVICE_MAX_RETRY = 10;
    private static final long RETRY_CONNECT_WAIT_TIME_MS = 1000;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.test_activity);

        //button deletefile
        btn_deleteFile = (Button) findViewById(R.id.DeleteFile);
        btn_deleteFile.setOnClickListener(this);

        btn_registerListener = (Button) findViewById(R.id.Registered);
        btn_registerListener.setOnClickListener(this);

        //创建binder对象
        mService = onCreatIDeleteFile();
        listener = new ClientListener();
    }

    
    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.DeleteFile:
                try {
                    mService.dealDeleteFiles("/data/data/com.example.android.IqtTestClient/xjtest");
                } catch(RemoteException e) {
                    Log.w(TAG,"Binder Communication dealDeleteFiles Failed : " + e.toString());
                }
            break;
            case R.id.Registered:
                try {
                    mService.registerLisntener(listener);
                } catch(RemoteException e) {
                    Log.w(TAG,"Binder Communication registerLisntener Failed : " + e.toString());
                }
            break;
        }
    }

    private static IDeleteFile onCreatIDeleteFile() {
        int retryCount = 0;
        IDeleteFile service = null;
        while(null == service) {
            Log.d(TAG,"MortalActivity::createVehicle() Service Name Is = " + IDeleteFile.class.getCanonicalName());
            service = IDeleteFile.Stub.asInterface(ServiceManager.getService(
                IDeleteFile.class.getCanonicalName()));
            if(++retryCount > CONNECT_SERVICE_MAX_RETRY) {
                break;
            }
            try {
                Thread.sleep(RETRY_CONNECT_WAIT_TIME_MS);
            } catch (InterruptedException e) {
                Log.w(TAG,"MortalActivity::createVehicle() Service Not Available: " + e.toString());
            }
        }
        if(null == service) {
            Log.w(TAG,"MortalActivity::createVehicle() Service Not Available");
        }
        return service;
    }

    private class ClientListener extends IClientListener.Stub {
        @Override
        public void notifyClientMessage(String msg) throws RemoteException {
            Log.d(TAG,"ClientListener::notifyClientMessage msg is = " + msg);
        }

        @Override
        public void registerCallBack(IServerCallBack callback) throws RemoteException {
            Log.d(TAG,"ClientListener::notifyClientMessage callback is = " + callback);
            mCallBack = callback;
            mCallBack.replyServerMessage("Client Reply Server CallBack");
        }
    }

}
