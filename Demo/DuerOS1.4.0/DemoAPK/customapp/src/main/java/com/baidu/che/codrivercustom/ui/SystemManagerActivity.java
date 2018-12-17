package com.baidu.che.codrivercustom.ui;

import com.baidu.che.codrivercustom.util.LogUtil;
import com.baidu.che.codrivercustom.util.ToastUtils;
import com.baidu.che.codrivercustom.widget.FuncButton;
import com.baidu.che.codriversdk.manager.CdCameraManager;
import com.baidu.che.codriversdk.manager.CdConfigManager;
import com.baidu.che.codriversdk.manager.CdSystemManager;

import android.os.Bundle;
import android.view.View;

/**
 * 系统控制
 */

public class SystemManagerActivity extends BaseActivity {
    private static final String TAG = "SystemManagerActivity";
    private static final boolean DEFAULT_RESULT = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addFunctionBtn(new FuncButton(mContext, "跳转到语音设置界面", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdSystemManager.getInstance().jumpToAsrSetting();
            }
        }), new FuncButton(mContext, "跳转到帮助设置界面", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdSystemManager.getInstance().jumpToHelpSetting();
            }
        }), new FuncButton(mContext, "设置行车记录仪控制工具", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdCameraManager.getInstance().setCameraTool(mRecordTool);
                ToastUtils.show("设置行车记录仪控制工具");
            }
        }));

        /**
         * 必须先执行setRecorderTool(...)/setSystemTool(...), 接入者才能接收到相关回调
         */
        addFunctionBtn(new FuncButton(mContext, "设置空调指令回调", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdSystemManager.getInstance().setACListener(mACListener);
                ToastUtils.show("设置空调指令回调");
            }
        }), new FuncButton(mContext, "设置车窗指令回调", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdSystemManager.getInstance().setCarWindowListener(mWindowListener);
                ToastUtils.show("设置车窗指令回调");
            }
        }), new FuncButton(mContext, "设置座椅指令回调", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdSystemManager.getInstance().setCarSeatListener(mSeatListener);
                ToastUtils.show("设置座椅指令回调");
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "设置车门指令回调", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdSystemManager.getInstance().setCarDoorListener(mDoorListener);
                ToastUtils.show("设置车门指令回调");
            }
        }), new FuncButton(mContext, "设置车灯指令回调", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdSystemManager.getInstance().setCarLightListener(mLightListener);
                ToastUtils.show("设置车灯指令回调");
            }
        }), new FuncButton(mContext, "设置系统指令回调", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdSystemManager.getInstance().setSystemSettingListener(mSystemSettingListener);
                ToastUtils.show("设置系统指令回调");
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "设置App指令回调", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdSystemManager.getInstance().setAppListener(mAppListener);
                ToastUtils.show("设置App指令回调");
            }
        }), new FuncButton(mContext, "设置摄像头指令回调", new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CdSystemManager.getInstance().setCarCameraListener(mCameraListener);
                ToastUtils.show("设置摄像头指令回调");
            }
        }));

        addFunctionBtn(new FuncButton(mContext, "打开dialog动画", new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                CdSystemManager.getInstance().openDialogAnimation();
            }
        }), new FuncButton(mContext, "关闭dialog动画", new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                CdSystemManager.getInstance().closeDialogAnimation();
            }
        }));
    }

    private CdCameraManager.DrivingRecorderTool mRecordTool = new CdCameraManager.DrivingRecorderTool() {
        @Override
        public void drivingRecorder(CdCameraManager.DrivingRecorderTool.DrivingRecorderState drivingRecorderState) {
            //操作录像
            switch (drivingRecorderState) {
                case WATCH:
                    ToastUtils.show("查看录像");
                    break;
                case START:
                    ToastUtils.show("开始录像");
                    break;
                case STOP:
                    ToastUtils.show("停止录像");
                    break;
                default:
                    break;
            }
        }

        @Override
        public void openCamera(CameraType cameraType) {
            //打开摄像头
            switch (cameraType) {
                case FRONT_CAMERA:
                    ToastUtils.show("前置摄像头");
                    break;
                case INNER_CAMERA:
                    ToastUtils.show("内置摄像头");
                    break;
                case BACK_CAMERA:
                    ToastUtils.show("后置摄像头");
                    break;
                default:
                    break;
            }
        }

        @Override
        public void takePicture() {
            ToastUtils.show("拍照");
        }
    };

    private CdSystemManager.ACListener mACListener = new CdSystemManager.ACListener() {
        // 在下面所有的回调方法内,如果厂商已经实现对相应功能的支持,则return true

        @Override
        public boolean openAirConditioner(CdSystemManager.DirectionType directionType) {
            LogUtil.d(TAG, "openAirConditioner() directionType = " + directionType + " ：true");
            ToastUtils.show("openAirConditioner() directionType = " + directionType + " ：true");
            return true;
        }

        @Override
        public boolean closeAirConditioner(CdSystemManager.DirectionType directionType) {
            LogUtil.d(TAG, "closeAirConditioner() directionType = " + directionType + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeAirConditioner() directionType = " + directionType + " ：" + DEFAULT_RESULT);
            CdConfigManager.getInstance().setCallbackTipString("该功能正在开发中");
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openAcLock() {
            LogUtil.d(TAG, "openAcLock()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openAcLock()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeAcLock() {
            LogUtil.d(TAG, "openAcLock()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openAcLock()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean setTemperature(CdSystemManager.MeasureType measureType,
                                      String s, CdSystemManager.DirectionType directionType) {
            LogUtil.d(TAG, "setTemperature() measureType = " + measureType
                    + ", s = " + s + ", directionType = " + directionType + " ：" + DEFAULT_RESULT);
            ToastUtils.show("setTemperature() measureType = " + measureType
                    + ", s = " + s + ", directionType = " + directionType + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openInternalRecycle() {
            LogUtil.d(TAG, "openInternalRecycle()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openInternalRecycle()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeInternalRecycle() {
            LogUtil.d(TAG, "closeInternalRecycle()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeInternalRecycle()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openExternalRecycle() {
            LogUtil.d(TAG, "openExternalRecycle()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openExternalRecycle()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeExternalRecycle() {
            LogUtil.d(TAG, "closeExternalRecycle()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeExternalRecycle()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean changeRecycleMode() {
            LogUtil.d(TAG, "changeRecycleMode()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("changeRecycleMode()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean setWindFlow(CdSystemManager.MeasureType measureType, String s,
                                   CdSystemManager.DirectionType directionType) {
            LogUtil.d(TAG, "setWindFlow() measureType = " + measureType + ", s = " + s
                    + ", directionType = " + directionType + " ：" + DEFAULT_RESULT);
            ToastUtils.show("setWindFlow() measureType = " + measureType + ", s = " + s
                    + ", directionType = " + directionType + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }


        @Override
        public boolean openCompressor() {
            LogUtil.d(TAG, "openCompressor()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openCompressor()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeCompressor() {
            LogUtil.d(TAG, "closeCompressor()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeCompressor()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean setWindMode(WindMode windMode) {
            LogUtil.d(TAG, "setWindMode() windMode = " + windMode + " ：" + DEFAULT_RESULT);
            ToastUtils.show("setWindMode() windMode = " + windMode + " ：" + DEFAULT_RESULT);
            return false;
        }

        @Override
        public boolean openACSettinsMode(ACSettinsMode acSettinsMode) {
            LogUtil.d(TAG, "openACSettinsMode() acSettinsMode = " + acSettinsMode + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openACSettinsMode() acSettinsMode = " + acSettinsMode + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeACSettinsMode(ACSettinsMode acSettinsMode) {
            LogUtil.d(TAG, "closeACSettinsMode() acSettinsMode = " + acSettinsMode + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeACSettinsMode() acSettinsMode = " + acSettinsMode + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean changeWindDirection(String s) {
            LogUtil.d(TAG, "changeWindDirection() s = " + s + " ：" + DEFAULT_RESULT);
            ToastUtils.show("changeWindDirection() s = " + s + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }
    };

    private CdSystemManager.CarSeatListener mSeatListener = new CdSystemManager.CarSeatListener() {
        @Override
        public boolean openHeat(SeatPosition seatPosition) {
            LogUtil.d(TAG, "openHeat(" + seatPosition + ")" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openHeat(" + seatPosition + ")" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeHeat(SeatPosition seatPosition) {
            LogUtil.d(TAG, "closeHeat(" + seatPosition + ")" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeHeat(" + seatPosition + ")" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openVent(SeatPosition seatPosition) {
            LogUtil.d(TAG, "openVent(" + seatPosition + ")" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openVent(" + seatPosition + ")" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeVent(SeatPosition seatPosition) {
            LogUtil.d(TAG, "closeVent(" + seatPosition + ")" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeVent(" + seatPosition + ")" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }


        @Override
        public boolean changeSeatPosition(CdSystemManager.MeasureType measureType,
                                          String s, SeatPosition seatPosition) {
            LogUtil.d(TAG, "changeSeatPosition() measureType = " + measureType
                    + ", s = " + s + ", seatPosition = " + seatPosition + " ：" + DEFAULT_RESULT);
            ToastUtils.show("changeSeatPosition() measureType = " + measureType
                    + ", s = " + s + ", seatPosition = " + seatPosition + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean seatbackForward() {
            LogUtil.d(TAG, "seatbackForward()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("seatbackForward()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean seatbackBackward() {
            LogUtil.d(TAG, "seatbackBackward()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("seatbackBackward()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean saveSeatPosition(int i) {
            LogUtil.d(TAG, "saveSeatPosition() i = " + i + " ：" + DEFAULT_RESULT);
            ToastUtils.show("saveSeatPosition() i = " + i);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean setSeatPosition(int i) {
            LogUtil.d(TAG, "setSeatPosition() i = " + i + " ：" + DEFAULT_RESULT);
            ToastUtils.show("setSeatPosition() i = " + i + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean setSeatTemperature(CdSystemManager.MeasureType measureType, String value, SeatPosition seatPosition) {
            LogUtil.d(TAG, "setSeatTemperature() measureType = " + measureType + ", value = "
                    + value + ", seatPosition = " + seatPosition + " ：" + DEFAULT_RESULT);
            ToastUtils.show("setSeatTemperature() measureType = " + measureType + ", value = "
                    + value + ", seatPosition = " + seatPosition + " ：" + DEFAULT_RESULT);
            return false;
        }
    };

    private CdSystemManager.CarWindowListener mWindowListener = new CdSystemManager.CarWindowListener() {

        @Override
        public boolean openWindow(CdSystemManager.DirectionType directionType, WindowOpenMode windowOpenMode) {
            LogUtil.d(TAG, "openWindow() directionType = " + directionType
                    + ", windowOpenMode = " + windowOpenMode + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openWindow() directionType = " + directionType
                    + ", windowOpenMode = " + windowOpenMode + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeWindow(CdSystemManager.DirectionType directionType) {
            LogUtil.d(TAG, "closeWindow() directionType = " + directionType + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeWindow() directionType = " + directionType + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openSkyWindow(int i) {
            LogUtil.d(TAG, "openSkyWindow() percent = " + i + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openSkyWindow() percent = " + i + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openSkyWindowByStep(boolean b) {
            LogUtil.d(TAG, "openSkyWindowByStep() isUp = " + b + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openSkyWindowByStep() isUp = " + b + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }


        @Override
        public boolean openSkyBoard() {
            LogUtil.d(TAG, "openSkyBoard()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openSkyBoard()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeSkyBoard() {
            LogUtil.d(TAG, "closeSkyBoard()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeSkyBoard()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openSkyWindowAngle() {
            LogUtil.d(TAG, "openSkyWindowAngle()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openSkyWindowAngle()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeSkyWindowAngle() {
            LogUtil.d(TAG, "closeSkyWindowAngle()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeSkyWindowAngle()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openSunBlind() {
            LogUtil.d(TAG, "openSunBlind()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openSunBlind()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeSunBlind() {
            LogUtil.d(TAG, "closeSunBlind()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeSunBlind()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }
    };

    private CdSystemManager.CarDoorListener mDoorListener = new CdSystemManager.CarDoorListener() {
        @Override
        public boolean openDoor() {
            LogUtil.d(TAG, "openDoor()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openDoor()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeDoor() {
            LogUtil.d(TAG, "closeDoor()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeDoor()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openTrunk() {
            LogUtil.d(TAG, "openTrunk()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openTrunk()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeTrunk() {
            LogUtil.d(TAG, "closeTrunk()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeTrunk()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean saveRearviewPosition(int i) {
            LogUtil.d(TAG, "saveRearviewPosition() i = " + i + " ：" + DEFAULT_RESULT);
            ToastUtils.show("saveRearviewPosition() i = " + i + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean setRearviewPosition(int i) {
            LogUtil.d(TAG, "setRearviewPosition() i = " + i + " ：" + DEFAULT_RESULT);
            ToastUtils.show("setRearviewPosition() i = " + i + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }
    };

    private CdSystemManager.CarLightListener mLightListener = new CdSystemManager.CarLightListener() {
        @Override
        public boolean openLight() {
            LogUtil.d(TAG, "openLight()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openLight()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeLight() {
            LogUtil.d(TAG, "closeLight()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeLight()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openCustomLight(CustomLightType customLightType) {
            LogUtil.d(TAG, "openCustomLight() customLightType = " + customLightType + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openCustomLight() customLightType = " + customLightType + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeCustomLight(CustomLightType customLightType) {
            LogUtil.d(TAG, "closeCustomLight() customLightType = " + customLightType + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeCustomLight() customLightType = " + customLightType + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }
    };

    private CdSystemManager.SystemSettingListener mSystemSettingListener = new CdSystemManager.SystemSettingListener() {
        @Override
        public boolean setVolume(CdSystemManager.MeasureType measureType, String s) {
            LogUtil.d(TAG, "setVolume() measureType = " + measureType + ", s = " + s + " ：" + DEFAULT_RESULT);
            ToastUtils.show("setVolume() measureType = " + measureType + ", s = " + s + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean setNaviVolume(CdSystemManager.MeasureType measureType, String s) {
            LogUtil.d(TAG, "setNaviVolume() measureType = " + measureType + ", s = " + s + " ：" + DEFAULT_RESULT);
            ToastUtils.show("setNaviVolume() measureType = " + measureType + ", s = " + s + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean setBrightness(CdSystemManager.MeasureType measureType, String s) {
            LogUtil.d(TAG, "setBrightness() measureType = " + measureType + ", s = " + s + " ：" + DEFAULT_RESULT);
            ToastUtils.show("setBrightness() measureType = " + measureType + ", s = " + s + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean backToLuancher() {
            LogUtil.d(TAG, "backToLuancher()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("backToLuancher()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean back() {
            LogUtil.d(TAG, "back()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("back()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean quitApp() {
            LogUtil.d(TAG, "quitApp()" + " ：" + DEFAULT_RESULT);
            ToastUtils.show("quitApp()" + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean openSystemSetting(SystemSetting systemSetting) {
            LogUtil.d(TAG, "openSystemSetting() systemSetting = " + systemSetting.name() + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openSystemSetting() systemSetting = " + systemSetting.name() + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeSystemSetting(SystemSetting systemSetting) {
            LogUtil.d(TAG, "closeSystemSetting() systemSetting = " + systemSetting.name() + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeSystemSetting() systemSetting = " + systemSetting.name() + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean connectBTDevice(String btDevices) {
            LogUtil.d(TAG, "connectBTDevice() btDevices = " + btDevices + " ：" + DEFAULT_RESULT);
            ToastUtils.show("connectBTDevice() btDevices = " + btDevices + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean disconnectBTDevice(String btDevices) {
            LogUtil.d(TAG, "disconnectBTDevice() btDevices = " + btDevices + " ：" + DEFAULT_RESULT);
            ToastUtils.show("disconnectBTDevice() btDevices = " + btDevices + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean deleteBTDevice(String btDevices) {
            LogUtil.d(TAG, "deleteBTDevice() btDevices = " + btDevices + " ：" + DEFAULT_RESULT);
            ToastUtils.show("deleteBTDevice() btDevices = " + btDevices + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }
    };

    private CdSystemManager.AppListener mAppListener = new CdSystemManager.AppListener() {
        @Override
        public boolean openApp(String s) {
            LogUtil.d(TAG, "openApp() s = " + s + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openApp() s = " + s + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeApp(String s) {
            LogUtil.d(TAG, "closeApp() s = " + s + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeApp() s = " + s + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean switchApp() {
            LogUtil.d(TAG, "switchApp() " + " ：" + DEFAULT_RESULT);
            ToastUtils.show("switchApp() " + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean displayFullScreenApp(String appName) {
            LogUtil.d(TAG, "displayFullScreenApp() appName = " + appName + " ：" + DEFAULT_RESULT);
            ToastUtils.show("displayFullScreenApp() appName = " + appName + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }
    };

    private CdSystemManager.CarCameraListener mCameraListener = new CdSystemManager.CarCameraListener() {
        @Override
        public boolean openCamera(CameraType cameraType) {
            LogUtil.d(TAG, "openCamera() cameraType = " + cameraType.name() + " ：" + DEFAULT_RESULT);
            ToastUtils.show("openCamera() cameraType = " + cameraType.name() + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean closeCamera(CameraType cameraType) {
            LogUtil.d(TAG, "closeCamera() cameraType = " + cameraType.name() + " ：" + DEFAULT_RESULT);
            ToastUtils.show("closeCamera() cameraType = " + cameraType.name() + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean takePicture(CameraType cameraType) {
            LogUtil.d(TAG, "takePicture() cameraType = " + cameraType.name() + " ：" + DEFAULT_RESULT);
            ToastUtils.show("takePicture() cameraType = " + cameraType.name() + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean startTakeVideo(CameraType cameraType) {
            LogUtil.d(TAG, "startTakeVideo() cameraType = " + cameraType.name() + " ：" + DEFAULT_RESULT);
            ToastUtils.show("startTakeVideo() cameraType = " + cameraType.name() + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean stopTakeVideo(CameraType cameraType) {
            LogUtil.d(TAG, "stopTakeVideo() cameraType = " + cameraType.name() + " ：" + DEFAULT_RESULT);
            ToastUtils.show("stopTakeVideo() cameraType = " + cameraType.name() + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }

        @Override
        public boolean watchVideo() {
            LogUtil.d(TAG, "watchVideo() " + " ：" + DEFAULT_RESULT);
            ToastUtils.show("watchVideo() " + " ：" + DEFAULT_RESULT);
            return DEFAULT_RESULT;
        }
    };
}
