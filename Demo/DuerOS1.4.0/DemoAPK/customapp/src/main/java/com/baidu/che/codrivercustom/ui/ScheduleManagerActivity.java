package com.baidu.che.codrivercustom.ui;

import android.os.Bundle;
import android.view.View;

import com.baidu.che.codrivercustom.util.LogUtil;
import com.baidu.che.codrivercustom.util.ToastUtils;
import com.baidu.che.codrivercustom.widget.FuncButton;
import com.baidu.che.codriversdk.manager.CdScheduleManager;

/**
 * 日程管理
 */

public class ScheduleManagerActivity extends BaseActivity {

    private static final String TAG = "ScheduleManagerActivity";
    private static final boolean DEFAULT_RESULT = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        /**
         * 接入该功能,必须先执行setScheduleListener(...),接入者才能接收到相关回调
         */
        addFunctionBtn(new FuncButton(mContext, "设置日程管理工具实例", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ToastUtils.show("设置日程管理工具实例");
                CdScheduleManager.getInstance().setScheduleListener(new MyScheduleListener());
            }
        }));
    }

    class MyScheduleListener extends CdScheduleManager.ScheduleListener {
        @Override
        public boolean openSchedule() {
            LogUtil.d(TAG, "openSchedule()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openSchedule()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openNextSchedule() {
            LogUtil.d(TAG, "openNextSchedule()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openNextSchedule()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }
    }

}
