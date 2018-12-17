package com.baidu.che.codrivercustom.widget;

import android.content.Context;
import android.widget.Button;

/**
 * 功能button
 */

public class FuncButton extends Button {

    public FuncButton(Context context, String btnName, OnClickListener listener) {
        super(context);
        this.setText(btnName);
        this.setTextSize(18);
        this.setOnClickListener(listener);
    }

}
