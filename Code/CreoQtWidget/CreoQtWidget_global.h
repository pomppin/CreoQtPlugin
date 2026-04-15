#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CREOQTWIDGET_LIB)
#  define CREOQTWIDGET_EXPORT Q_DECL_EXPORT
# else
#  define CREOQTWIDGET_EXPORT Q_DECL_IMPORT
# endif
#else
# define CREOQTWIDGET_EXPORT
#endif

#include <QApplication>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLibrary>
#include <QMessageBox>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <ProArray.h>
#include <ProAsmcomp.h>
#include <ProAsmcomppath.h>
#include <ProDimension.h>
#include <ProDrawing.h>
#include <ProFeatType.h>
#include <ProFeature.h>
#include <ProMdl.h>
#include <ProMenu.h>
#include <ProMenuBar.h>
#include <ProMessage.h>
#include <ProModelitem.h>
#include <ProNotify.h>
#include <ProParameter.h>
#include <ProParamval.h>
#include <ProPopupmenu.h>
#include <ProRefInfo.h>
#include <ProRelSet.h>
#include <ProSelbuffer.h>
#include <ProSelection.h>
#include <ProSolid.h>
#include <ProSurface.h>
#include <ProToolkit.h>
#include <ProToolkitDll.h>
#include <ProUICmd.h>
#include <ProUIMessage.h>
#include <ProUtil.h>
#include <ProWindows.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif