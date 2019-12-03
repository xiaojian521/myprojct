package jp.pioneer.ceam.thirdpartyadapter;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.HashMap;

import jp.pioneer.ceam.thirdpartyadapter.urlhttp.CallBackUtil;
import jp.pioneer.ceam.thirdpartyadapter.urlhttp.UrlHttpUtil;

public class MainActivity extends AppCompatActivity {
    private Button mButton;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mButton = (Button) findViewById(R.id.button);
        mButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getData();

                //postData();
            }
        });

    }

    private void postData() {
        String url = "https://www.baidu.com/";
        HashMap<String, String> paramsMap = new HashMap<>();
        paramsMap.put("title","title");
        paramsMap.put("desc","desc");
        UrlHttpUtil.post(url, paramsMap, new CallBackUtil.CallBackString() {
            @Override
            public void onFailure(int code, String errorMessage) {

            }

            @Override
            public void onResponse(String response) {
                Toast.makeText(MainActivity.this,"Success",Toast.LENGTH_SHORT).show();
                Log.d("xjtest",response);
            }
        });
    }

    private void getData() {
        String url = "https://www.baidu.com/";
        UrlHttpUtil.get(url, new CallBackUtil.CallBackString() {
            @Override
            public void onFailure(int code, String errorMessage) {

            }

            @Override
            public void onResponse(String response) {
                Toast.makeText(MainActivity.this,"Success",Toast.LENGTH_SHORT).show();
                Log.d("xjtest",response);
            }
        });
    }

}
