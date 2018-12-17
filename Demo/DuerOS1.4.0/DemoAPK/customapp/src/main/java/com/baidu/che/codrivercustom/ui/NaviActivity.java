package com.baidu.che.codrivercustom.ui;

import com.baidu.che.codrivercustom.util.LogUtil;
import com.baidu.che.codrivercustom.util.ToastUtils;
import com.baidu.che.codrivercustom.widget.FuncButton;
import com.baidu.che.codriversdk.manager.CdNaviManager;

import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

/**
 * 导航模块
 */

public class NaviActivity extends BaseActivity {

    private static final String TAG = "NaviActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        init();

        /**
         * 接入该功能,必须先执行setNaviTool(...),接入者才能接收到相关回调
         */
        addFunctionBtn(new FuncButton(mContext, "设置导航工具", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().setNaviStatusTool(new CdNaviManager.NaviStatusTool() {
                    @Override
                    public void onReceivedNaviStatus(CdNaviManager.NaviStatus status) {
                        LogUtil.d(TAG, "onReceivedNaviStatus() : " + status.name());
                        Toast.makeText(mContext, status.name(), Toast.LENGTH_LONG).show();
                    }

                    @Override
                    public void onReceivedPoiUpdate(CdNaviManager.PoiModel poiModel) {
                        LogUtil.d("NaviActivity", "onReceivedPoiUpdate() poiType : " + poiModel.poiType
                                + ", diatance : " + poiModel.poiDistance + ", poiAddress : " + poiModel.poiAddress
                                + ", poiName : " + poiModel.poiName + ", latitude" + poiModel.latitude
                                + ", longitude : " + poiModel.longitude + ", status : " + poiModel.status
                                + ", share_url : " + poiModel.shareUrl + ", province : " + poiModel.province
                                + ", city : " + poiModel.city);
                        Toast.makeText(mContext, poiModel.poiName, Toast.LENGTH_LONG).show();
                    }
                });
                ToastUtils.show("设置导航工具");
            }
        }), new FuncButton(mContext, "设置百度地图为导航app", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().setDefaultNaviApp(CdNaviManager.NaviApp.Baidu);
                ToastUtils.show("设置百度地图为导航app");
            }
        }), new FuncButton(mContext, "设置高德地图后视镜版为导航app", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().setDefaultNaviApp(CdNaviManager.NaviApp.Amap_Lite);
                ToastUtils.show("设置后视镜版高德地图为导航app");
            }
        }), new FuncButton(mContext, "设置车机版高德地图为导航app", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().setDefaultNaviApp(CdNaviManager.NaviApp.Amap);
                ToastUtils.show("设置高德地图车机版为导航app");
            }
        }), new FuncButton(mContext, "发起导航", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.PoiModel poiModel = new CdNaviManager.PoiModel();
                poiModel.poiName = "";  //POI名称
                poiModel.latitude = 1;  //纬度(必填)
                poiModel.longitude = 2; //经度(必填)
                poiModel.poiAddress = "";   //地址
                boolean status = CdNaviManager.getInstance().sendStartNaviCommand(poiModel);
                ToastUtils.show(status ? "发起导航" : "数据有问题,请重新输入");
            }
        }), new FuncButton(mContext, "退出导航", new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                CdNaviManager.getInstance().closeMap();
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "设置地图为白天模式", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().setDayOrNightMode(true);
                ToastUtils.show("设置地图为白天模式");
            }
        }), new FuncButton(mContext, "设置地图为黑夜模式", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().setDayOrNightMode(false);
                ToastUtils.show("设置地图为黑夜模式");
            }
        }));
        addFunctionBtn(new FuncButton(mContext, "放大地图", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().zoomMap(true);
                ToastUtils.show("放大地图");
            }
        }), new FuncButton(mContext, "缩小地图", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().zoomMap(false);
                ToastUtils.show("缩小地图");
            }
        }), new FuncButton(mContext, "是否导航中", new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                CdNaviManager.getInstance().isInNavi(new CdNaviManager.IsNaviCallback() {
                    @Override
                    public void isInNavi(boolean isNavi) {
                        ToastUtils.show(isNavi ? "正在导航" : "没有导航");
                    }
                });
            }
        }));

        //尝试触发设置或取消地图为黑夜模式，
        //当地图为自动模式时生效（如果强制设置了地图为白天/黑夜模式，此方法不生效）
        addTitle(20, "尝试触发设置或取消地图为黑夜模式", null);

        addFunctionBtn(new FuncButton(mContext, "触发黑夜模式", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().triggerNightMode(true);
                ToastUtils.show("触发黑夜模式");
            }
        }), new FuncButton(mContext, "取消黑夜模式", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().triggerNightMode(false);
                ToastUtils.show("取消黑夜模式");
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "设置家的位置", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.PoiModel address = new CdNaviManager.PoiModel();
                address.poiName = "宝安";
                address.type = CdNaviManager.AddressType.home.name();
                address.poiAddress = "坪洲";
                address.longitude = 112;
                address.latitude = 28;
                CdNaviManager.getInstance().setAppointAddress(address);
            }
        }), new FuncButton(mContext, "设置公司位置", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.PoiModel address = new CdNaviManager.PoiModel();
                address.poiName = "南山";
                address.type = CdNaviManager.AddressType.office.name();
                address.poiAddress = "百度";
                address.longitude = 116;
                address.latitude = 40;
                CdNaviManager.getInstance().setAppointAddress(address);
            }
        }), new FuncButton(mContext, "获取家的位置", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().getAppointAddress(CdNaviManager.AddressType.home,
                        new CdNaviManager.AddressCallback() {
                            @Override
                            public void onResultAddress(CdNaviManager.PoiModel address) {
                                if (address != null) {
                                    ToastUtils.show("家的地址 = " + address.poiName);
                                }
                            }
                        });
            }
        }), new FuncButton(mContext, "获取公司位置", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().getAppointAddress(CdNaviManager.AddressType.office,
                        new CdNaviManager.AddressCallback() {
                            @Override
                            public void onResultAddress(CdNaviManager.PoiModel address) {
                                if (address != null) {
                                    ToastUtils.show("公司的地址 = " + address.poiName);
                                }
                            }
                        });
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "导航去家", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().sendStartNaviHomeOrAddress(CdNaviManager.AddressType.home);
            }
        }), new FuncButton(mContext, "导航去公司", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().sendStartNaviHomeOrAddress(CdNaviManager.AddressType.office);
            }
        }), new FuncButton(mContext, "导航去任意地点", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdNaviManager.getInstance().sendAddress(CdNaviManager.PoiType.GCJ02, null, startPoi, endPoi, passPoi);
            }
        }));
    }

    private CdNaviManager.PoiModel startPoi = new CdNaviManager.PoiModel();
    private CdNaviManager.PoiModel endPoi = new CdNaviManager.PoiModel();
    private List<CdNaviManager.PoiModel> passPoi = new ArrayList<>();

    private void init() {
        startPoi.poiName = "南山";
        startPoi.longitude = 116;
        startPoi.latitude = 40;
        startPoi.poiDistance = "1000";
        startPoi.poiType = CdNaviManager.PoiType.GCJ02;

        endPoi.poiName = "百度科技园";
        endPoi.longitude = 116.27355;
        endPoi.latitude = 40.04352;

        passPoi.clear();

        CdNaviManager.PoiModel pass1 = new CdNaviManager.PoiModel();
        pass1.poiName = "湖北";
        pass1.longitude = 117;
        pass1.latitude = 41;
        passPoi.add(pass1);

        CdNaviManager.PoiModel pass2 = new CdNaviManager.PoiModel();
        pass2.poiName = "河南";
        pass2.longitude = 118;
        pass2.latitude = 42;
        passPoi.add(pass2);

    }


}
