# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
CMAKE_MINIMUM_REQUIRED(VERSION 3.15.0 FATAL_ERROR)

# 设置 CMake 策略，消除 CMP0177 警告
IF(POLICY CMP0177)
    CMAKE_POLICY(SET CMP0177 NEW)
ENDIF()

# 设置项目名称
PROJECT(CreoQtWidget)

# 设置c++语言标准，这里使用c++11
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译器编码为utf8
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /we4715")
# SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8 /we4715")

# 设置启用QT编译器
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)


# 设置是否存在顶层项目CMAKE的全局标志
SET(IS_EXIST_PROJECT_CMAKE ON)

SET(Qt5_DIR "C:/Qt/Qt5.15.2/5.15.2/msvc2019_64/lib/cmake/Qt5" CACHE PATH "Set Qt5 directory")
SET(CREO_COMMON_FILES_DIR "C:/Program Files/PTC/Creo 4.0/M040/Common Files" CACHE PATH "Set Creo Common Files directory")
SET(CREO_PROTOOLKIT_INCLUDES_DIR "${CREO_COMMON_FILES_DIR}/protoolkit/includes" CACHE PATH "Set ProToolkit includes directory")
SET(CREO_PROTOOLKIT_LIB_DIR "${CREO_COMMON_FILES_DIR}/protoolkit/x86e_win64/obj" CACHE PATH "Set ProToolkit lib directory")

# 检查第三方依赖库目录是否存在
IF(NOT EXISTS ${Qt5_DIR})        
    MESSAGE(SEND_ERROR "Please specify Qt5 location!~ Qt5_DIR = ${Qt5_DIR}")
ENDIF()
IF(NOT EXISTS ${CREO_COMMON_FILES_DIR})        
    MESSAGE(SEND_ERROR "Please specify Creo Common Files location!~ CREO_COMMON_FILES_DIR = ${CREO_COMMON_FILES_DIR}")
ENDIF()
IF(NOT EXISTS ${CREO_PROTOOLKIT_INCLUDES_DIR})        
    MESSAGE(SEND_ERROR "Please specify Creo ProToolkit includes location!~ CREO_PROTOOLKIT_INCLUDES_DIR = ${CREO_PROTOOLKIT_INCLUDES_DIR}")
ENDIF()
IF(NOT EXISTS ${CREO_PROTOOLKIT_LIB_DIR})        
    MESSAGE(SEND_ERROR "Please specify Creo ProToolkit lib location!~ CREO_PROTOOLKIT_LIB_DIR = ${CREO_PROTOOLKIT_LIB_DIR}")
ENDIF()

# 包含Creo头文件
INCLUDE_DIRECTORIES(${CREO_PROTOOLKIT_INCLUDES_DIR})

# 引入依赖库
LINK_DIRECTORIES(${CREO_PROTOOLKIT_LIB_DIR})

# 包含当前目录
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# 设置项目文件的输出目录
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(PROJECT_BUILD_TYPE_POSTFIX "d")
ELSE()
    SET(PROJECT_BUILD_TYPE_POSTFIX "")
ENDIF()

# 将构建后缀设置为全局变量，让子目录可见
SET(PROJECT_BUILD_TYPE_POSTFIX ${PROJECT_BUILD_TYPE_POSTFIX} CACHE INTERNAL "Build type postfix")

# 设置编译选项
OPTION(USE_PROJECT_OUTPUT_DIR "Project file output dir" OFF)

IF(USE_PROJECT_OUTPUT_DIR)
    SET(PROJECT_OUTPUT_DIR ${USE_PROJECT_OUTPUT_DIR})
ELSE()
    SET(PROJECT_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../output)
ENDIF()
SET(PROJECT_MODULE_OUTPUT_DIR ${PROJECT_OUTPUT_DIR}/bin${PROJECT_BUILD_TYPE_POSTFIX})

# 设置库文件输出位置
SET(LIBRARY_OUTPUT_PATH ${PROJECT_MODULE_OUTPUT_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_MODULE_OUTPUT_DIR})

# 设置全局UTF-8编码
IF(MSVC)
    ADD_COMPILE_OPTIONS("/source-charset:utf-8" "/execution-charset:utf-8")
ENDIF()

SET(CUSTOM_INSTALL_PREFIX ${PROJECT_OUTPUT_DIR})

# 生成插件配置文件
CONFIGURE_FILE(
    ${PROJECT_SOURCE_DIR}/Resources/CreoQtWidget.dat.in
    ${PROJECT_OUTPUT_DIR}/CreoQtWidget.dat
    @ONLY
)

# 安装插件到指定目录
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Resources/text" DESTINATION "${PROJECT_OUTPUT_DIR}")

FILE(COPY ${PROJECT_SOURCE_DIR}/Resources/text DESTINATION ${PROJECT_OUTPUT_DIR})

FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

GET_FILENAME_COMPONENT(QT_LIB_DIR "${Qt5_DIR}" DIRECTORY) # .../lib/cmake
GET_FILENAME_COMPONENT(QT_LIB_DIR "${QT_LIB_DIR}" DIRECTORY) # .../lib 
GET_FILENAME_COMPONENT(QT_ROOT_DIR "${QT_LIB_DIR}" DIRECTORY) # .../msvc2019_64 (根目录)
 
SET(QT_BIN_DIR "${QT_ROOT_DIR}/bin")
SET(QT_PLUGINS_DIR "${QT_ROOT_DIR}/plugins")
 
MESSAGE(STATUS "Qt Root Dir: ${QT_ROOT_DIR}")
MESSAGE(STATUS "Qt Bin Dir: ${QT_BIN_DIR}")
 
# 2. 定义部署函数 (直接定义，不写入文件)
FUNCTION(COPY_QT_LIBRARIES TARGET_NAME)
    # 使用生成器表达式自动判断 Debug/Release 后缀 
    SET(DLL_SUFFIX "$<IF:$<CONFIG:Debug>,d,>")
    
    # 复制核心 DLL 
    ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "${QT_BIN_DIR}/Qt5Core${DLL_SUFFIX}.dll"
            "${QT_BIN_DIR}/Qt5Gui${DLL_SUFFIX}.dll"
            "${QT_BIN_DIR}/Qt5Widgets${DLL_SUFFIX}.dll"
            $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMENT "Copying Qt Core DLLs for config: $<CONFIG>"
    )
 
    # 确保 platforms 目录存在 
    ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/platforms"
    )
 
    # 仅复制匹配当前编译模式的平台插件 
    ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "${QT_PLUGINS_DIR}/platforms/qwindows${DLL_SUFFIX}.dll"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/platforms/"
        COMMENT "Copying Qt platform plugin: qwindows${DLL_SUFFIX}.dll"
    )
 
    # 设置延迟加载 
    IF(MSVC)
        TARGET_LINK_OPTIONS(${TARGET_NAME} PRIVATE 
            /DELAYLOAD:Qt5Core.dll 
            /DELAYLOAD:Qt5Gui.dll 
            /DELAYLOAD:Qt5Widgets.dll 
        )
        TARGET_LINK_LIBRARIES(${TARGET_NAME} PRIVATE delayimp)
    ENDIF()
ENDFUNCTION()

# 检查关键ProToolkit库文件是否存在
SET(REQUIRED_PROTOOLKIT_LIBS
    "${CREO_PROTOOLKIT_LIB_DIR}/protk_dll_NU.lib"
    "${CREO_PROTOOLKIT_LIB_DIR}/protk_dllmd_NU.lib"
)

FOREACH(LIB_FILE ${REQUIRED_PROTOOLKIT_LIBS})
    IF(NOT EXISTS ${LIB_FILE})
        MESSAGE(SEND_ERROR "Missing required ProToolkit library: ${LIB_FILE}")
    ENDIF()
ENDFOREACH()

# 添加子目录
ADD_SUBDIRECTORY(CreoQtWidget)