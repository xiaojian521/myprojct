package com.example.mortal;

import com.example.mortal.IClientListener;

interface IDeleteFile {
    int dealDeleteFiles(String path)                     = 0;
    void registerLisntener(IClientListener listener)     = 1;
}