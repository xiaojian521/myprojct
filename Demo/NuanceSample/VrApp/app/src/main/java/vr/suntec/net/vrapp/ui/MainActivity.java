package vr.suntec.net.vrapp.ui;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import vr.suntec.net.vrapp.R;
import vr.suntec.net.vrapp.vrService.TtsEngine.NuanceEngine;

public class MainActivity extends Activity {
    Button m_startspeak;
    NuanceEngine m_ttsengin;
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        try {

            finalize();
        }catch (Throwable a) {
        }
        setContentView(R.layout.activity_main);
//
//        // Example of a call to a native method
//        TextView tv = (TextView) findViewById(R.id.sample_text);
//        tv.setText(stringFromJNI());

        m_startspeak = (Button)findViewById(R.id.button1);
//
        m_ttsengin = new NuanceEngine();
        m_ttsengin.onTtsCreate(this);

        m_startspeak.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                m_ttsengin.onStartSpeak("上海今天天天气刷机还看我多大叔大婶大大王大无打我的爱的哇多爱我的哇大啊大王大王大好的空间哈文皇帝开挖后第卡号大空间和维护的空间按我的扩大化我卡和空间大货款及安徽","text_id");
            }
        });


    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}


