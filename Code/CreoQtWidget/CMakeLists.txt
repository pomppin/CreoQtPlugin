# CMakeList.txt: CreoQtWidget 的 CMake 项目，在此处包括源代码并定义

#=========================配置项目信息=======================================

# 项目名称
PROJECT(CreoQtWidget LANGUAGES CXX)

# 设置启用QT编译器
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)

# 设置Qt依赖库
SET(QTCOMPONENT Core Gui Widgets)

# =================引入依赖库==================

FIND_PACKAGE(Qt5 REQUIRED ${QTCOMPONENT})

# 引入所有H文件和CPP文件
FILE(GLOB SRC_H_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
SOURCE_GROUP("include" FILES ${SRC_H_FILES})

FILE(GLOB SRC_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
SOURCE_GROUP("source" FILES ${SRC_CPP_FILES})

ADD_DEFINITIONS(-DPRO_USE_VAR_ARGS)

# 包含顶层传递的函数文件
# INCLUDE("${PROJECT_SOURCE_DIR}/copy_qt_libraries.cmake")

# 增加动态链接库
ADD_LIBRARY(${PROJECT_NAME} SHARED
	${SRC_H_FILES}
	${SRC_CPP_FILES})

TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE CREOQTWIDGET_LIB)
	
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    PRIVATE
	Qt5::Core 
	Qt5::Widgets 
	Qt5::Gui
	netapi32
	psapi
	mpr
	ws2_32
	protk_dllmd_NU
	ucore
	udata
	)

# 调用顶层定义的复制Qt库函数
COPY_QT_LIBRARIES(${PROJECT_NAME})

# 设置安装目标
INSTALL(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ${PROJECT_MODULE_OUTPUT_DIR}
    LIBRARY DESTINATION ${PROJECT_MODULE_OUTPUT_DIR}
)
