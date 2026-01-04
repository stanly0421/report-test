// 引入自定義的 Widget 類別標頭檔
#include "widget.h"

// 引入 Qt 應用程式框架的標頭檔
#include <QApplication>

// 主程式進入點，接收命令列參數
int main(int argc, char *argv[])
{
    // 創建 Qt 應用程式物件，傳入命令列參數
    QApplication a(argc, argv);
    // 創建主視窗 Widget 物件
    Widget w;
    // 顯示主視窗
    w.show();
    // 進入應用程式的主事件迴圈，等待使用者操作
    return a.exec();
}
