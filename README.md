# CreoQtPlugin
使用Qt CMake开发的Creo插件示例

编译生成之后，如果要调整插件的路径，需要在生成目录下修改CreoQtWidget.dat里面的路径（EXEC_FILE和TEXT_DIR）：

```
NAME CreoQtWidget
STARTUP DLL
EXEC_FILE C:/output/bin/CreoQtWidget.dll
TEXT_DIR C:/output/text/zh_cn
ALLOW_STOP TRUE
REVISION 1.0
END
```

EXEC_FILE为插件动态库路径，TEXT_DIR为菜单栏中文翻译和说明文件（MenuBarText.txt）所在目录。
