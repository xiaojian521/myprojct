package com.example.android.IqtTestClient;

import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.view.View;
import android.widget.Button;
import android.os.RemoteException;
import android.util.Log;
import android.os.Parcel;
import android.os.ServiceManager;

import java.io.IOException;
import com.example.android.IqtTestClient.TLogUtils;

public class MortalActivity extends Activity {
    static final private String TAG = "xjtest";
    private final static int DELETE_FILES = 0x1; 

    private Button registerListener;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.test_activity);
        //add button click
        registerListener = (Button)findViewById(R.id.REGISTELISTENER);
        registerListener.setOnClickListener(new RegisterListenerClick());
    }

    //button RegisterListenerClick
    private class RegisterListenerClick implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            deleteLogs();
        }
    }

    //start delete log file
    private void deleteLogs() {
        final String delete_path = "/data/data/com.example.MortalClient./xjtest";
        Parcel data = Parcel.obtain();
        data.writeString(delete_path);

        IBinder binder = null;
        final int MAX_WAIT = 9;//最多获取service 10次
        int index = 0;
        do {
            binder = ServiceManager.getService("xjtest_server");
            if(null == binder) {
                try {
                    Thread.sleep(100);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        } while( (null == binder) && ((index++) < MAX_WAIT) );

        if(null == binder) {
            Log.d("xjtest","TLogUtils::dealUseRoot() get service of autoaction failed");
            data.recycle();
            return false;
        }

        Parcel reply = Parcel.obtain();
        int result = -1;
        try {
            binder.transact(DELETE_FILES,data,reply,0);
            result = reply.readInt();
        } catch (Exception e) {
            Log.d("xjtest"," send cmd to autoaction service : error" + e.toString());
        }
        data.recycle();
        reply.recycle();
    }

}
