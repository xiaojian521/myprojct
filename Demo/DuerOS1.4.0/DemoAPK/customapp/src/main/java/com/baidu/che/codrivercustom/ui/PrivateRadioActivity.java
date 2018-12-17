package com.baidu.che.codrivercustom.ui;

import android.os.Bundle;
import android.view.View;

import com.baidu.che.codrivercustom.util.LogUtil;
import com.baidu.che.codrivercustom.util.ToastUtils;
import com.baidu.che.codrivercustom.widget.FuncButton;
import com.baidu.che.codriversdk.manager.CdPrivateRadioManager;

public class PrivateRadioActivity extends BaseActivity {

    private static final String TAG = "PrivateRadioActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        /**
         * 接入该功能,setPrivateRadioTool(...),接入者才能接收到相关回调
         */
        addFunctionBtn(new FuncButton(mContext, "设置有声节目工具实例", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdPrivateRadioManager.getInstance().setPrivateRadioTool(privateRadioTool);
                ToastUtils.show("设置有声节目工具实例");
            }
        }), new FuncButton(mContext, "重置有声节目工具实例", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdPrivateRadioManager.getInstance().resetPrivateRadioTool();
                ToastUtils.show("重置有声节目工具实例");
            }
        }));
    }

    private CdPrivateRadioManager.PrivateRadioTool privateRadioTool = new CdPrivateRadioManager.PrivateRadioTool() {

        @Override
        public boolean openPrivateRadio(CdPrivateRadioManager.PrivateRadioModel privateRadioModel) {
            ToastUtils.show("打开有声节目失败 openPrivateRadio()");
            LogUtil.d(TAG, "openPrivateRadio()");
            return false;
            // ToastUtils.show("打开有声节目成功 openPrivateRadio()");
            // return true;
        }

        @Override
        public boolean openRadioByChannel(String s) {
            LogUtil.d(TAG, "openRadioByChannel() : " + s);
            ToastUtils.show("打开有声节目失败 openRadioByChannel() : " + s);
            return false;
            // ToastUtils.show("打开在线电台频道成功 openRadioByChannel() : " + s);
            // return true;
        }

        @Override
        public boolean openOnlineRadio() {
            LogUtil.d(TAG, "openOnlineRadio()");
            ToastUtils.show("打开有声节目失败 openPrivateRadio()");
            return false;
            // ToastUtils.show("打开在线电台成功 openOnlineRadio()");
            // return true;
        }
    };
}
