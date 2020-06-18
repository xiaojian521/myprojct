package com.example.mortal;

import com.example.mortal.IServerCallBack;

interface IClientListener {
    void notifyClientMessage(String msg)                    = 1;
    void registerCallBack(IServerCallBack callback)         = 2;
}