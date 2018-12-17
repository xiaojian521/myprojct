package com.baidu.che.codrivercustom.ui;

import com.baidu.che.codrivercustom.R;
import com.baidu.che.codrivercustom.util.CommonUtils;
import com.baidu.che.codrivercustom.widget.FuncButton;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * 基类Activity
 */

public class BaseActivity extends Activity {

    //获取上下文
    protected Context mContext;
    //控制控件布局
    protected LinearLayout mLLContent;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_common);

        mContext = this;
        Intent intent = getIntent();
        String title = "";
        if (intent != null) {
            title = intent.getStringExtra("title");
        }
        if (TextUtils.isEmpty(title)) {
            title = "示例demo";
        }
        ((TextView) findViewById(R.id.txt_title)).setText(title);
        mLLContent = (LinearLayout) findViewById(R.id.ll_content);
        findViewById(R.id.img_back).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                finish();
            }
        });
    }

    protected void addFunctionBtn(FuncButton... funcButtons) {
        LinearLayout linearLayout = new LinearLayout(mContext);
        mLLContent.addView(linearLayout);

        int screenWidth = CommonUtils.getScreenWidth(mContext);
        LinearLayout.LayoutParams lp =
                new LinearLayout.LayoutParams(screenWidth / funcButtons.length - 20,
                        ViewGroup.LayoutParams.WRAP_CONTENT);

        for (int i = 0; i < funcButtons.length; i++) {
            FuncButton btn = funcButtons[i];
            FrameLayout fm = new FrameLayout(mContext);
            fm.setPadding(10, 10, 10, 10);
            fm.addView(btn, lp);
            linearLayout.addView(fm);
        }
    }

    /**
     * 添加标题
     *
     * @param txtSize
     * @param name
     * @param params
     */
    protected void addTitle(int txtSize, String name, LinearLayout.LayoutParams params) {
        TextView textView = new TextView(mContext);
        textView.setTextSize(txtSize);
        textView.setText(name);
        if (params != null) {
            mLLContent.addView(textView, params);
        } else {
            LinearLayout.LayoutParams lp =
                    new LinearLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT,
                            ViewGroup.LayoutParams.WRAP_CONTENT);
            lp.gravity = Gravity.CENTER;
            mLLContent.addView(textView, lp);
        }
    }
}
