package vr.suntec.net.vrapp;

import android.app.Application;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import vr.suntec.net.vrapp.vrService.TtsEngine.NuanceEngine;

public class VrApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(VRCommonDef.VRLogTag, "VrApplication::onCreate");
        // Create DM
        //xj
        //m_ttsengin = new NuanceEngine();
        //m_ttsengin.onTtsCreate(getApplicationContext());
//        m_startTts = (Button) m_startTts.findViewById(R.id.button);
//        m_startTts.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
               // m_ttsengin.onStartSpeak("哈哈哈,肖健","1");
//            }
//        });
    }
}
