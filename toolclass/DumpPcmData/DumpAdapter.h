#ifndef DUMPADAPTER_H
#define DUMPADAPTER_H

#include "CommonDefine.h"

#ifdef __cplusplus
extern "C" {
#endif

int Dump(int argv, char** argc);

void Start(char* type);

void Stop(char* type);

void PostMessage(char* type, MessageInfo msg);

#ifdef __cplusplus
}
#endif

#endif