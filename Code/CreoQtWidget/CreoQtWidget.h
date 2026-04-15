#pragma once

#include "CreoQtWidget_global.h"

// 前向声明，减少头文件依赖 
class QApplication;
class QDialog;

class CREOQTWIDGET_EXPORT CreoQtWidget
{
public:
    // 公共入口点，供全局函数调用
    static int Initialize();
    static void Terminate();

private:
    // 禁止实例化 (纯静态管理类)
    CreoQtWidget() = delete;
    ~CreoQtWidget() = delete;

    // --- 内部逻辑函数 ---

    // 环境初始化 
    static void SetupQtEnvironment();

    // Creo 命令回调函数 (静态成员函数可作为 C 回调指针)
    static uiCmdAccessState AccessDefault(uiCmdAccessMode access_mode);

    // 具体功能实现 
    static void OnTest1();
    static void OnTest2();

    // 辅助工具 
    static QString GetErrorStr(ProError err);

    // --- 静态成员变量 (替代全局变量) ---
    static QApplication* m_app;         // Qt 应用程序对象
    static QDialog* m_currentDlg;       // 当前非模态对话框指针
    static int m_argc;                  // QApplication 参数
    static char* m_argv[];              // QApplication 参数 
};