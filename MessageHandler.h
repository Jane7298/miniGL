#ifndef _MessageHandler_H
#define _MessageHandler_H

///消息处理入口点
LRESULT handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

///栅格化模式标志
extern bool g_IsRasterizeMode;

#endif