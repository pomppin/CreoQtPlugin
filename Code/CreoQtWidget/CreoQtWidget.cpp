#include "CreoQtWidget.h"
#include <windows.h>
#include <string>
#include <cstdlib>

// --- 静态成员变量定义 (必须在类外定义) ---
QApplication* CreoQtWidget::m_app = nullptr;
QDialog* CreoQtWidget::m_currentDlg = nullptr;
int CreoQtWidget::m_argc = 1;
char* CreoQtWidget::m_argv[] = { (char*)"xtop.exe" };

// --- 公共入口点实现 ---

int CreoQtWidget::Initialize()
{
    // 1. 配置环境变量 (必须在创建 QApplication 之前)
    SetupQtEnvironment();

    // 2. 创建 QApplication
    if (nullptr == m_app) {
        m_app = new QApplication(m_argc, m_argv);
    }

    // 3. 注册 Creo 菜单和命令 
    ProError status;
    ProFileName MsgFile;
    ProStringToWstring(MsgFile, (char*)"MenuBarText.txt");

    uiCmdCmdId PushButton1_cmd_id, PushButton2_cmd_id;

    // 添加菜单 
    status = ProMenubarMenuAdd((char*)"MainMenu", (char*)"Function", (char*)"Help", PRO_B_TRUE, MsgFile);

    // 注册命令1: 使用类的静态成员函数 OnTest1 作为回调 
    ProCmdActionAdd((char*)"PushButton1_Act", (uiCmdCmdActFn)OnTest1, 12, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &PushButton1_cmd_id);
    ProMenubarmenuPushbuttonAdd((char*)"MainMenu", (char*)"PushButton", (char*)"FirstButton", (char*)"this button will show a message", NULL, PRO_B_TRUE, PushButton1_cmd_id, MsgFile);

    // 注册命令2: 使用类的静态成员函数 OnTest2 作为回调 
    ProCmdActionAdd((char*)"PushButton2_Act", (uiCmdCmdActFn)OnTest2, uiCmdPrioDefault, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &PushButton2_cmd_id);
    ProMenubarmenuPushbuttonAdd((char*)"MainMenu", (char*)"PushButton2", (char*)"SecondButton", (char*)"this button will show a message", NULL, PRO_B_TRUE, PushButton2_cmd_id, MsgFile);

    return status;
}

void CreoQtWidget::Terminate()
{
    if (nullptr != m_app) {
        m_app->closeAllWindows();
        m_app->quitOnLastWindowClosed();
        m_app->exit();
        m_app->deleteLater();
        delete m_app;
        m_app = nullptr;
    }
}

void CreoQtWidget::SetupQtEnvironment()
{
    char path[MAX_PATH];
    HMODULE hm = NULL;

    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&CreoQtWidget::SetupQtEnvironment, &hm)) {

        GetModuleFileNameA(hm, path, MAX_PATH);

        std::string dllPath(path);
        size_t pos = dllPath.find_last_of("\\/");
        if (pos != std::string::npos) {
            dllPath = dllPath.substr(0, pos);
        }

        _putenv_s("QT_PLUGIN_PATH", dllPath.c_str());

        char* oldPath = getenv("PATH");
        std::string newPath = dllPath + ";" + (oldPath ? oldPath : "");
        _putenv_s("PATH", newPath.c_str());
    }
}

uiCmdAccessState CreoQtWidget::AccessDefault(uiCmdAccessMode access_mode)
{
    return ACCESS_AVAILABLE;
}

QString CreoQtWidget::GetErrorStr(ProError err)
{
    switch (err) {
    case PRO_TK_NO_ERROR: return "Success";
    case PRO_TK_BAD_INPUTS: return "Bad Inputs";
    case PRO_TK_NOT_EXIST: return "Not Exist";
    default: return QString("Error Code: %1").arg(err);
    }
}

// 获取模型信息
void CreoQtWidget::OnTest1()
{
    // 清理旧的对话框
    if (nullptr != m_currentDlg) {
        delete m_currentDlg;
        m_currentDlg = nullptr;
    }

    ProMdl currentMdl = nullptr;
    ProError status = ProMdlCurrentGet(&currentMdl);

    QString displayMsg;

    if (status != PRO_TK_NO_ERROR || currentMdl == nullptr) {
        displayMsg = "Error: No active model found!\nPlease open a part or assembly.";
    }
    else {
        ProName mdlNameW;
        ProMdlNameGet(currentMdl, mdlNameW);
        QString modelName = QString::fromWCharArray(mdlNameW);

        ProMdlType mdlType;
        ProMdlTypeGet(currentMdl, &mdlType);
        QString typeStr = (mdlType == PRO_MDL_PART) ? "Part" :
            (mdlType == PRO_MDL_ASSEMBLY) ? "Assembly" : "Other";

        displayMsg = QString("Model: %1\nType: %2\n\n").arg(modelName, typeStr);

        if (mdlType == PRO_MDL_PART || mdlType == PRO_MDL_ASSEMBLY) {
            ProMassProperty mass_prop;
            status = ProSolidMassPropertyGet((ProSolid)currentMdl, NULL, &mass_prop);

            if (status == PRO_TK_NO_ERROR) {
                displayMsg += "--- Geometry Statistics ---\n";
                displayMsg += QString("Volume:       %1\n").arg(mass_prop.volume, 0, 'f', 4);
                displayMsg += QString("Surface Area: %1\n").arg(mass_prop.surface_area, 0, 'f', 4);
                displayMsg += QString("Density:      %1\n").arg(mass_prop.density, 0, 'f', 4);

                if (mass_prop.density > 0) {
                    displayMsg += QString("Mass:         %1\n").arg(mass_prop.mass, 0, 'f', 4);
                }
                else {
                    displayMsg += "Mass:         N/A (Density is 0)\n";
                }

                displayMsg += QString("\nCenter of Gravity:\n");
                displayMsg += QString("  X: %1\n").arg(mass_prop.center_of_gravity[0], 0, 'f', 4);
                displayMsg += QString("  Y: %1\n").arg(mass_prop.center_of_gravity[1], 0, 'f', 4);
                displayMsg += QString("  Z: %1\n").arg(mass_prop.center_of_gravity[2], 0, 'f', 4);
            }
            else {
                displayMsg += QString("Calculation Failed: %1\n").arg(GetErrorStr(status));
                displayMsg += "Ensure the model has valid solid geometry.";
            }
        }
        else {
            displayMsg += "Geometry calculation is only supported for Parts and Assemblies.";
        }
    }

    // 创建并显示对话框
    m_currentDlg = new QDialog;
    m_currentDlg->setWindowTitle("Geometry Statistics");
    m_currentDlg->resize(400, 300);

    QVBoxLayout* layout = new QVBoxLayout(m_currentDlg);
    QLabel* lblInfo = new QLabel(displayMsg, m_currentDlg);
    lblInfo->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lblInfo->setWordWrap(true);

    QFont font("Consolas", 10);
    lblInfo->setFont(font);

    layout->addWidget(lblInfo);
    m_currentDlg->setLayout(layout);

    m_currentDlg->show();
}

// 测试模态对话框
void CreoQtWidget::OnTest2()
{
    QDialog dlg;
    dlg.setWindowTitle("B");
    dlg.setMinimumHeight(300);
    dlg.setMinimumWidth(500);
    dlg.exec();
}

// --- 全局 C 接口 (Creo 入口) ---
// 这两个函数保持全局，仅作为桥梁调用类的静态方法

extern "C" int user_initialize() {
    return CreoQtWidget::Initialize();
}

extern "C" void user_terminate() {
    CreoQtWidget::Terminate();
}