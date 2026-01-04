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
// 引入 Qt 外部程序處理類別
#include <QProcess>
// 引入 Qt 多行文字編輯器類別
#include <QTextEdit>
// 引入 Qt 文字瀏覽器類別（支援 HTML）
#include <QTextBrowser>
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

// 影片/音樂資訊結構
struct VideoInfo {
    QString videoId;          // YouTube 影片 ID (用於 YouTube 連結)
    QString filePath;         // 本地檔案路徑 (用於本地音樂)
    QString title;            // 影片/音樂標題
    QString channelTitle;     // 頻道名稱/藝術家
    QString thumbnailUrl;     // 縮圖 URL
    QString description;      // 描述
    QString subtitlePath;     // 字幕檔案路徑 (SRT 檔案)
    bool isFavorite;          // 是否為喜愛的影片/音樂
    bool isLocalFile;         // 是否為本地檔案
};

// 播放清單結構
struct Playlist {
    QString name;              // 播放清單名稱
    QList<VideoInfo> videos;   // 影片列表
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
    // 載入字幕檔案按鈕點擊處理函式
    void onLoadSubtitleFileClicked();
    
    // 播放清單項目雙擊處理函式
    void onVideoDoubleClicked(QListWidgetItem* item);
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
    
    // Whisper 輸出準備就緒處理函式
    void onWhisperOutputReady();
    // Whisper 完成處理函式
    void onWhisperFinished(int exitCode, QProcess::ExitStatus exitStatus);
    
    // 字幕連結點擊處理函式（跳轉到指定時間）
    void onSubtitleLinkClicked(const QUrl& url);

private:
    // 設定使用者介面的函式
    void setupUI();
    // 建立信號與槽連接的函式
    void createConnections();
    // 更新播放清單顯示的函式
    void updatePlaylistDisplay();
    // 更新目標播放清單下拉選單的函式
    void updateTargetPlaylistComboBox();
    // 播放指定索引的影片/音樂
    void playVideo(int index);
    // 更新按鈕啟用/停用狀態的函式
    void updateButtonStates();
    // 儲存播放清單到檔案的函式
    void savePlaylistsToFile();
    // 從檔案載入播放清單的函式
    void loadPlaylistsFromFile();
    // 取得下一首影片/音樂的索引
    int getNextVideoIndex();
    // 取得隨機影片/音樂的索引，excludeCurrent 決定是否排除當前播放的項目
    int getRandomVideoIndex(bool excludeCurrent = true);
    // 取得未播放的影片/音樂索引清單
    QList<int> getUnplayedVideoIndices(bool excludeCurrent = true);
    // 播放 YouTube 連結的函式
    void playYouTubeLink(const QString& link);
    // 播放本地檔案的函式
    void playLocalFile(const QString& filePath);
    // 從 URL 提取 YouTube 影片 ID 的函式
    QString extractYouTubeVideoId(const QString& url);
    // 產生 YouTube 顯示用的 HTML 內容
    QString generateYouTubeDisplayHTML(const QString& title, const QString& channel, const QString& videoId);
    // 產生本地音樂顯示用的 HTML 內容
    QString generateLocalMusicHTML(const QString& title, const QString& fileName);
    // 產生歡迎畫面的 HTML 內容
    QString generateWelcomeHTML();
    // 更新影片資訊標籤的函式
    void updateVideoLabels(const VideoInfo& video);
    // 建立影片顯示用的 HTML 內容
    QString createVideoDisplayHTML(const VideoInfo& video);
    // 啟動 Whisper 語音轉錄的函式
    void startWhisperTranscription(const QString& audioFilePath);
    // 載入 SRT 字幕檔案的函式
    void loadSrt(const QString& srtFilePath);
    // 更新當前播放影片的字幕顯示
    void updateSubtitleDisplay();
    // 恢復當前影片標題的函式
    void restoreCurrentVideoTitle();
    // 更新本地音樂顯示區域，包含字幕內容
    void updateLocalMusicDisplay(const QString& title, const QString& fileName, const QString& subtitles);
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
    
    // 影片顯示區域 - 使用 QTextBrowser 顯示內容和字幕
    QTextBrowser* videoDisplayArea;
    
    // Whisper 語音轉錄外部程序物件指標
    QProcess* whisperProcess;
    // 當前 SRT 字幕檔案的路徑
    QString currentSrtFilePath;
    
    // 載入本地檔案的按鈕指標
    QPushButton* loadLocalFileButton;
    // 載入字幕檔案的按鈕指標
    QPushButton* loadSubtitleButton;
    // 影片標題標籤指標
    QLabel* videoTitleLabel;
    // 頻道名稱標籤指標
    QLabel* channelLabel;
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
    // 當前播放的影片/音樂索引
    int currentVideoIndex;
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
    // 當前會話中已播放的影片/音樂索引集合
    QSet<int> playedVideosInCurrentSession;
    // 用於解析字幕時間戳的正則表達式
    QRegularExpression subtitleTimestampRegex;
    // 用於解析 SRT 格式時間戳的正則表達式
    QRegularExpression srtTimestampRegex;
    // 用於識別 SRT 序號行的正則表達式
    QRegularExpression sequenceNumberRegex;
    // 累積的字幕內容，用於整合顯示在主視窗
    QString currentSubtitles;
    // 用於恢復影片標題的計時器（在字幕跳轉通知後）
    QTimer* titleRestoreTimer;
};

// 結束標頭檔保護宏
#endif // WIDGET_H
