// 防止標頭檔重複引入的保護宏
#ifndef WIDGET_H
#define WIDGET_H

// 引入 Qt 基本視窗元件類別
#include <QWidget>
// 引入 Qt 按鈕元件類別
#include <QPushButton>
// 引入 Qt 標籤元件類別
#include <QLabel>
// 引入 Qt 滑桿元件類別
#include <QSlider>
// 引入 Qt 清單視窗元件類別
#include <QListWidget>
// 引入 Qt 清單視窗項目類別
#include <QListWidgetItem>
// 引入 Qt 下拉式選單元件類別
#include <QComboBox>
// 引入 Qt 單行文字輸入框元件類別
#include <QLineEdit>
// 引入 Qt 輸入對話框類別
#include <QInputDialog>
// 引入 Qt 訊息對話框類別
#include <QMessageBox>
// 引入 Qt 時間處理類別
#include <QTime>
// 引入 Qt URL 處理類別
#include <QUrl>
// 引入 Qt 映射容器類別（鍵值對）
#include <QMap>
// 引入 Qt 清單容器類別
#include <QList>
// 引入 Qt 集合容器類別（不重複元素）
#include <QSet>
// 引入 Qt 媒體播放器類別
#include <QMediaPlayer>
// 引入 Qt 音訊輸出類別
#include <QAudioOutput>
// 引入 Qt 檔案對話框類別
#include <QFileDialog>
// 引入 Qt JSON 文件類別
#include <QJsonDocument>
// 引入 Qt JSON 物件類別
#include <QJsonObject>
// 引入 Qt JSON 陣列類別
#include <QJsonArray>
// 引入 Qt 計時器類別
#include <QTimer>
// 引入 Qt 事件處理類別
#include <QEvent>
// Qt 命名空間起始標記
QT_BEGIN_NAMESPACE
// 前向宣告 Ui 命名空間中的 Widget 類別
namespace Ui {
class Widget;
}
// Qt 命名空間結束標記
QT_END_NAMESPACE

// 音樂資訊結構
struct MusicInfo {
    QString filePath;         // 本地檔案路徑 (用於本地音樂)
    QString title;            // 音樂標題
    QString artist;           // 藝術家
    bool isFavorite;          // 是否為喜愛的音樂
};

// 播放清單結構
struct Playlist {
    QString name;              // 播放清單名稱
    QList<MusicInfo> songs;    // 音樂列表
};

// Widget 類別，繼承自 QWidget，並使用 Q_OBJECT 宏啟用 Qt 的信號槽機制
class Widget : public QWidget
{
    Q_OBJECT

public:
    // 建構函式，parent 參數用於設定父視窗
    Widget(QWidget *parent = nullptr);
    // 解構函式，用於清理資源
    ~Widget();

private slots:
    // 播放/暫停按鈕點擊處理函式
    void onPlayPauseClicked();
    // 上一首按鈕點擊處理函式
    void onPreviousClicked();
    // 下一首按鈕點擊處理函式
    void onNextClicked();
    // 隨機播放按鈕點擊處理函式
    void onShuffleClicked();
    // 循環播放按鈕點擊處理函式
    void onRepeatClicked();
    
    // 載入本地檔案按鈕點擊處理函式
    void onLoadLocalFileClicked();
    
    // 播放清單項目雙擊處理函式
    void onSongDoubleClicked(QListWidgetItem* item);
    // 加入播放清單按鈕點擊處理函式
    void onAddToPlaylistClicked();
    // 從播放清單刪除項目的處理函式
    void onDeleteFromPlaylist();
    // 播放清單右鍵選單處理函式
    void onPlaylistContextMenu(const QPoint& pos);
    
    // 新增播放清單按鈕點擊處理函式
    void onNewPlaylistClicked();
    // 刪除播放清單按鈕點擊處理函式
    void onDeletePlaylistClicked();
    // 播放清單切換處理函式
    void onPlaylistChanged(int index);
    
    // 媒體播放器狀態改變處理函式
    void onMediaPlayerStateChanged();
    // 媒體播放器播放位置改變處理函式
    void onMediaPlayerPositionChanged(qint64 position);
    // 媒體播放器總時長改變處理函式
    void onMediaPlayerDurationChanged(qint64 duration);
    
    // 進度條按下處理函式
    void onProgressSliderPressed();
    // 進度條釋放處理函式
    void onProgressSliderReleased();
    // 進度條移動處理函式
    void onProgressSliderMoved(int position);
    // 音量滑桿改變處理函式
    void onVolumeSliderChanged(int value);
    // 音量標籤點擊處理函式（靜音/取消靜音）
    void onVolumeLabelClicked();

private:
    // 設定使用者介面的函式
    void setupUI();
    // 建立信號與槽連接的函式
    void createConnections();
    // 更新播放清單顯示的函式
    void updatePlaylistDisplay();
    // 更新目標播放清單下拉選單的函式
    void updateTargetPlaylistComboBox();
    // 播放指定索引的音樂
    void playMusic(int index);
    // 更新按鈕啟用/停用狀態的函式
    void updateButtonStates();
    // 儲存播放清單到檔案的函式
    void savePlaylistsToFile();
    // 從檔案載入播放清單的函式
    void loadPlaylistsFromFile();
    // 取得下一首音樂的索引
    int getNextMusicIndex();
    // 取得隨機音樂的索引，excludeCurrent 決定是否排除當前播放的項目
    int getRandomMusicIndex(bool excludeCurrent = true);
    // 取得未播放的音樂索引清單
    QList<int> getUnplayedMusicIndices(bool excludeCurrent = true);
    // 播放本地檔案的函式
    void playLocalFile(const QString& filePath);
    // 更新音樂資訊標籤的函式
    void updateMusicLabels(const MusicInfo& music);
    // 恢復當前音樂標題的函式
    void restoreCurrentMusicTitle();
    // 根據音量等級更新音量圖示
    void updateVolumeIcon(int volume);
    
protected:
    // 事件過濾器，用於處理特定物件的事件
    bool eventFilter(QObject *obj, QEvent *event) override;

    // Qt Designer 產生的 UI 物件指標
    Ui::Widget *ui;
    
    // Qt 媒體播放器物件指標
    QMediaPlayer* mediaPlayer;
    // Qt 音訊輸出物件指標
    QAudioOutput* audioOutput;
    
    // 載入本地檔案的按鈕指標
    QPushButton* loadLocalFileButton;
    // 音樂標題標籤指標
    QLabel* musicTitleLabel;
    // 藝術家名稱標籤指標
    QLabel* artistLabel;
    // 播放/暫停按鈕指標
    QPushButton* playPauseButton;
    // 上一首按鈕指標
    QPushButton* previousButton;
    // 下一首按鈕指標
    QPushButton* nextButton;
    // 隨機播放按鈕指標
    QPushButton* shuffleButton;
    // 循環播放按鈕指標
    QPushButton* repeatButton;
    // 加入播放清單按鈕指標
    QPushButton* addToPlaylistButton;
    // 目標播放清單下拉選單指標
    QComboBox* targetPlaylistComboBox;
    // 新增播放清單按鈕指標
    QPushButton* newPlaylistButton;
    // 刪除播放清單按鈕指標
    QPushButton* deletePlaylistButton;
    // 播放清單視窗元件指標
    QListWidget* playlistWidget;
    // 播放清單選擇下拉選單指標
    QComboBox* playlistComboBox;
    // 播放進度條滑桿指標
    QSlider* progressSlider;
    // 當前時間標籤指標
    QLabel* currentTimeLabel;
    // 總時長標籤指標
    QLabel* totalTimeLabel;
    // 音量滑桿指標
    QSlider* volumeSlider;
    // 音量圖示標籤指標
    QLabel* volumeLabel;
    
    // 所有播放清單的清單
    QList<Playlist> playlists;
    // 當前選中的播放清單索引
    int currentPlaylistIndex;
    // 當前播放的音樂索引
    int currentMusicIndex;
    // 是否啟用隨機播放模式
    bool isShuffleMode;
    // 是否啟用循環播放模式
    bool isRepeatMode;
    // 是否正在播放
    bool isPlaying;
    // 追蹤進度條是否被使用者按下
    bool isProgressSliderPressed;
    // 追蹤是否處於靜音狀態
    bool isMuted;
    // 保存靜音前的音量值
    int previousVolume;
    // 追蹤是否正在手動切換歌曲
    bool isSwitchingSongs;
    // 上次使用的播放清單名稱
    QString lastPlaylistName;
    // 當前會話中已播放的音樂索引集合
    QSet<int> playedSongsInCurrentSession;
    // 用於恢復音樂標題的計時器（在跳轉通知後）
    QTimer* titleRestoreTimer;
};

// 結束標頭檔保護宏
#endif // WIDGET_H
