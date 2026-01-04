// 引入自定義的 Widget 類別標頭檔
#include "widget.h"
// 引入 UI 自動產生的標頭檔
#include "ui_widget.h"
// 引入 Qt 垂直佈局管理器類別
#include <QVBoxLayout>
// 引入 Qt 水平佈局管理器類別
#include <QHBoxLayout>
// 引入 Qt 網格佈局管理器類別
#include <QGridLayout>
// 引入 Qt 群組框類別
#include <QGroupBox>
// 引入 Qt 檔案處理類別
#include <QFile>
// 引入 Qt 檔案資訊類別
#include <QFileInfo>
// 引入 Qt 目錄處理類別
#include <QDir>
// 引入 Qt 隨機數產生器類別
#include <QRandomGenerator>
// 引入 Qt 標準路徑取得類別
#include <QStandardPaths>
// 引入 Qt 分割視窗類別
#include <QSplitter>
// 引入 Qt 正則表達式類別
#include <QRegularExpression>
// 引入 Qt 文字瀏覽器類別
#include <QTextBrowser>
// 引入 Qt 文字串流類別
#include <QTextStream>
// 引入 Qt 外部程序類別
#include <QProcess>
// 引入 Qt 桌面服務類別
#include <QDesktopServices>
// 引入 Qt 計時器類別
#include <QTimer>
// 引入 Qt 選單類別
#include <QMenu>
// 引入 Qt 滑鼠事件類別
#include <QMouseEvent>
// 引入 C++ 數學函式庫
#include <cmath>

// 匿名命名空間，將 SongSwitchGuard 類別限制在此翻譯單元內
namespace {
    // RAII 輔助類別，用於管理 isSwitchingSongs 旗標
    class SongSwitchGuard {
    public:
        // 建構函式，接收旗標參考並將其設為 true
        explicit SongSwitchGuard(bool& flag) : m_flag(flag) {
            // 設定旗標為 true，表示正在切換歌曲
            m_flag = true;
        }
        // 解構函式，自動將旗標設回 false
        ~SongSwitchGuard() {
            // 重置旗標為 false，表示歌曲切換完成
            m_flag = false;
        }
        // 防止複製建構函式（刪除）
        SongSwitchGuard(const SongSwitchGuard&) = delete;
        // 防止複製賦值運算子（刪除）
        SongSwitchGuard& operator=(const SongSwitchGuard&) = delete;
        // 防止移動建構函式（刪除）
        SongSwitchGuard(SongSwitchGuard&&) = delete;
        // 防止移動賦值運算子（刪除）
        SongSwitchGuard& operator=(SongSwitchGuard&&) = delete;
    private:
        // 儲存旗標的參考
        bool& m_flag;
    };
}

// Widget 類別的建構函式，初始化所有成員變數
Widget::Widget(QWidget *parent)
    : QWidget(parent)  // 呼叫父類別的建構函式
    , ui(new Ui::Widget)  // 創建 UI 物件
    , mediaPlayer(new QMediaPlayer(this))  // 創建媒體播放器物件
    , audioOutput(new QAudioOutput(this))  // 創建音訊輸出物件
    , currentPlaylistIndex(-1)  // 初始化當前播放清單索引為 -1（無選擇）
    , currentMusicIndex(-1)  // 初始化當前音樂索引為 -1（無選擇）
    , isShuffleMode(false)  // 初始化隨機播放模式為關閉
    , isRepeatMode(false)  // 初始化循環播放模式為關閉
    , isPlaying(false)  // 初始化播放狀態為停止
    , isProgressSliderPressed(false)  // 初始化進度條按下狀態為否
    , isMuted(false)  // 初始化靜音狀態為否
    , previousVolume(50)  // 初始化先前音量為 50%
    , isSwitchingSongs(false)  // 初始化切換歌曲旗標為否
    , titleRestoreTimer(new QTimer(this))  // 創建標題恢復計時器物件
{
    // 設定 UI 元件
    ui->setupUi(this);
    
    // 設置媒體播放器，將音訊輸出連接到播放器
    mediaPlayer->setAudioOutput(audioOutput);
    // 設定音訊輸出音量為 50%（0.5）
    audioOutput->setVolume(0.5);
    
    // 設置標題恢復計時器為單次觸發
    titleRestoreTimer->setSingleShot(true);
    // 連接計時器逾時信號到恢復標題的槽函式
    connect(titleRestoreTimer, &QTimer::timeout, this, &Widget::restoreCurrentMusicTitle);
    
    // 設置主視窗標題
    setWindowTitle("音樂播放器");
    // 設置主視窗最小尺寸為 1000x700
    setMinimumSize(1000, 700);
    
    // 呼叫函式建立使用者介面
    setupUI();
    
    // 呼叫函式建立信號與槽的連接
    createConnections();
    
    // 從檔案載入已保存的播放清單
    loadPlaylistsFromFile();
    
    // 檢查是否沒有任何播放清單
    if (playlists.isEmpty()) {
        // 創建預設播放清單物件
        Playlist defaultPlaylist;
        // 設定播放清單名稱
        defaultPlaylist.name = "我的播放清單";
        // 將播放清單加入清單中
        playlists.append(defaultPlaylist);
        
        // 創建我的最愛播放清單物件
        Playlist favoritesPlaylist;
        // 設定播放清單名稱
        favoritesPlaylist.name = "我的最愛";
        // 將播放清單加入清單中
        playlists.append(favoritesPlaylist);
        
        // 將預設播放清單名稱加入到下拉選單
        playlistComboBox->addItem(defaultPlaylist.name);
        // 將我的最愛播放清單名稱加入到下拉選單
        playlistComboBox->addItem(favoritesPlaylist.name);
        // 設定當前播放清單索引為 0（第一個播放清單）
        currentPlaylistIndex = 0;
    } else {
        // 如果已有播放清單，恢復播放清單到下拉選單
        // 遍歷所有播放清單
        for (const Playlist& playlist : playlists) {
            // 將播放清單名稱加入到下拉選單
            playlistComboBox->addItem(playlist.name);
        }
        
        // 恢復上次使用的播放清單
        // 初始化上次使用的索引為 0
        int lastIndex = 0;
        // 遍歷所有播放清單尋找上次使用的播放清單
        for (int i = 0; i < playlists.size(); i++) {
            // 如果播放清單名稱與上次使用的名稱相符
            if (playlists[i].name == lastPlaylistName) {
                // 記錄索引
                lastIndex = i;
                // 跳出迴圈
                break;
            }
        }
        // 設定下拉選單的當前索引為上次使用的索引
        playlistComboBox->setCurrentIndex(lastIndex);
        // 更新當前播放清單索引
        currentPlaylistIndex = lastIndex;
        // 更新播放清單顯示
        updatePlaylistDisplay();
    }
    
    // 更新目標播放清單下拉選單（用於加入歌曲到其他播放清單）
    updateTargetPlaylistComboBox();
    
    // 更新所有按鈕的啟用/停用狀態
    updateButtonStates();
}

// Widget 類別的解構函式，負責清理資源
Widget::~Widget()
{
    // 將播放清單儲存到檔案
    savePlaylistsToFile();
    // 刪除 UI 物件，釋放記憶體
    delete ui;
}

// 設定使用者介面的函式，建立所有 UI 元件和佈局
void Widget::setupUI()
{
    // 創建主垂直佈局管理器，並設定為此視窗的佈局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    // 設定佈局內元件間距為 0
    mainLayout->setSpacing(0);
    // 設定佈局邊距為 0（左、上、右、下）
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 設置深色主題樣式表
    setStyleSheet(
        // 所有 QWidget 元件的基本樣式
        "QWidget {"
        // 設定背景顏色為深灰黑色
        "   background-color: #121212;"
        // 設定文字顏色為白色
        "   color: #FFFFFF;"
        "}"
        // QLineEdit（單行文字輸入框）的樣式
        "QLineEdit {"
        // 設定背景顏色為較淺的灰色
        "   background-color: #282828;"
        // 設定邊框為 1px 實線深灰色
        "   border: 1px solid #404040;"
        // 設定圓角半徑為 20px
        "   border-radius: 20px;"
        // 設定內邊距（上下 8px，左右 16px）
        "   padding: 8px 16px;"
        // 設定文字顏色為白色
        "   color: #FFFFFF;"
        // 設定字體大小為 14px
        "   font-size: 14px;"
        "}"
        // QLineEdit 獲得焦點時的樣式
        "QLineEdit:focus {"
        // 設定邊框顏色為 Spotify 綠色
        "   border: 1px solid #1DB954;"
        "}"
        // QListWidget（清單視窗）的樣式
        "QListWidget {"
        // 設定背景顏色為深灰色
        "   background-color: #181818;"
        // 移除邊框
        "   border: none;"
        // 移除選取框
        "   outline: none;"
        "}"
        // QListWidget 項目的樣式
        "QListWidget::item {"
        // 設定內邊距為 10px
        "   padding: 10px;"
        // 設定底部邊框為 1px 實線深灰色
        "   border-bottom: 1px solid #282828;"
        // 設定文字顏色為淺灰色
        "   color: #B3B3B3;"
        "}"
        // QListWidget 項目滑鼠懸停時的樣式
        "QListWidget::item:hover {"
        // 設定背景顏色為較深的灰色
        "   background-color: #282828;"
        // 設定文字顏色為白色
        "   color: #FFFFFF;"
        "}"
        // QListWidget 項目被選取時的樣式
        "QListWidget::item:selected {"
        // 設定背景顏色為 Spotify 綠色
        "   background-color: #1DB954;"
        // 設定文字顏色為白色
        "   color: #FFFFFF;"
        "}"
        // QComboBox（下拉選單）的樣式
        "QComboBox {"
        // 設定背景顏色為深灰色
        "   background-color: #282828;"
        // 設定邊框為 1px 實線深灰色
        "   border: 1px solid #404040;"
        // 設定圓角半徑為 4px
        "   border-radius: 4px;"
        // 設定內邊距為 8px
        "   padding: 8px;"
        // 設定文字顏色為白色
        "   color: #FFFFFF;"
        // 設定最小寬度為 150px
        "   min-width: 150px;"
        "}"
        // QComboBox 下拉按鈕的樣式
        "QComboBox::drop-down {"
        // 移除邊框
        "   border: none;"
        "}"
        // QComboBox 下拉清單的樣式
        "QComboBox QAbstractItemView {"
        // 設定背景顏色為深灰色
        "   background-color: #282828;"
        // 設定文字顏色為白色
        "   color: #FFFFFF;"
        // 設定選取項目的背景顏色為 Spotify 綠色
        "   selection-background-color: #1DB954;"
        "}"
    );
    
    // === 頂部搜尋欄 ===
    QWidget* topBar = new QWidget(this);
    topBar->setStyleSheet("background-color: #000000; padding: 16px;");
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    
    QLabel* logoLabel = new QLabel("🎵 音樂播放器", topBar);
    logoLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #1DB954;");
    topLayout->addWidget(logoLabel);
    
    topLayout->addStretch();
    
    loadLocalFileButton = new QPushButton("📁 載入音樂檔案", topBar);
    loadLocalFileButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 8px 24px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #404040; }"
        "QPushButton:pressed { background-color: #505050; }"
    );
    topLayout->addWidget(loadLocalFileButton);
    
    mainLayout->addWidget(topBar);
    
    // === 內容區域 ===
    QSplitter* contentSplitter = new QSplitter(Qt::Horizontal, this);
    contentSplitter->setStyleSheet("QSplitter::handle { background-color: #282828; }");
    
    // === 左側面板：播放清單 ===
    QWidget* leftPanel = new QWidget(contentSplitter);
    leftPanel->setStyleSheet("background-color: #000000;");
    leftPanel->setMinimumWidth(250);
    leftPanel->setMaximumWidth(350);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(16, 16, 16, 16);
    leftLayout->setSpacing(12);
    
    QLabel* playlistLabel = new QLabel("播放清單", leftPanel);
    playlistLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF; margin-bottom: 8px;");
    leftLayout->addWidget(playlistLabel);
    
    playlistComboBox = new QComboBox(leftPanel);
    leftLayout->addWidget(playlistComboBox);
    
    QHBoxLayout* playlistButtonLayout = new QHBoxLayout();
    
    newPlaylistButton = new QPushButton("➕ 新增", leftPanel);
    newPlaylistButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
    );
    playlistButtonLayout->addWidget(newPlaylistButton);
    
    deletePlaylistButton = new QPushButton("🗑️ 刪除", leftPanel);
    deletePlaylistButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
    );
    playlistButtonLayout->addWidget(deletePlaylistButton);
    
    leftLayout->addLayout(playlistButtonLayout);
    
    playlistWidget = new QListWidget(leftPanel);
    playlistWidget->setDragDropMode(QAbstractItemView::InternalMove);
    playlistWidget->setDefaultDropAction(Qt::MoveAction);
    playlistWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    leftLayout->addWidget(playlistWidget);
    
    contentSplitter->addWidget(leftPanel);
    
    // === 中央面板：影片播放器和搜尋結果 ===
    QWidget* centerPanel = new QWidget(contentSplitter);
    centerPanel->setStyleSheet("background-color: #121212;");
    QVBoxLayout* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(16, 16, 16, 16);
    centerLayout->setSpacing(16);
    
    // 音樂資訊
    musicTitleLabel = new QLabel("選擇一首歌曲開始播放", centerPanel);
    musicTitleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFFFFF;");
    musicTitleLabel->setWordWrap(true);
    centerLayout->addWidget(musicTitleLabel);
    
    artistLabel = new QLabel("", centerPanel);
    artistLabel->setStyleSheet("font-size: 14px; color: #B3B3B3;");
    centerLayout->addWidget(artistLabel);
    
    // 添加一些空間
    centerLayout->addSpacing(50);
    
    // 播放進度條區域
    QWidget* progressWidget = new QWidget(centerPanel);
    progressWidget->setStyleSheet("background-color: transparent;");
    QHBoxLayout* progressLayout = new QHBoxLayout(progressWidget);
    progressLayout->setContentsMargins(0, 0, 0, 0);
    progressLayout->setSpacing(8);
    
    currentTimeLabel = new QLabel("00:00", progressWidget);
    currentTimeLabel->setStyleSheet("color: #B3B3B3; font-size: 12px; min-width: 45px;");
    progressLayout->addWidget(currentTimeLabel);
    
    progressSlider = new QSlider(Qt::Horizontal, progressWidget);
    progressSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   border: none;"
        "   height: 4px;"
        "   background: #404040;"
        "   border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #1DB954;"
        "   border: none;"
        "   width: 12px;"
        "   height: 12px;"
        "   margin: -4px 0;"
        "   border-radius: 6px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "   background: #1ED760;"
        "}"
        "QSlider::sub-page:horizontal {"
        "   background: #1DB954;"
        "   border-radius: 2px;"
        "}"
    );
    progressSlider->setEnabled(false);
    progressLayout->addWidget(progressSlider, 1);
    
    totalTimeLabel = new QLabel("00:00", progressWidget);
    totalTimeLabel->setStyleSheet("color: #B3B3B3; font-size: 12px; min-width: 45px;");
    progressLayout->addWidget(totalTimeLabel);
    
    centerLayout->addWidget(progressWidget);
    
    // 播放控制區域
    QWidget* controlWidget = new QWidget(centerPanel);
    controlWidget->setStyleSheet("background-color: #181818; border-radius: 8px; padding: 16px;");
    QHBoxLayout* controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setSpacing(12);
    
    QString buttonStyle = 
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #FFFFFF;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   min-width: 40px;"
        "}"
        "QPushButton:hover { background-color: #404040; }"
        "QPushButton:pressed { background-color: #505050; }"
        "QPushButton:disabled { background-color: #181818; color: #404040; }";
    
    shuffleButton = new QPushButton("🔀", controlWidget);
    shuffleButton->setStyleSheet(buttonStyle);
    shuffleButton->setCheckable(true);
    shuffleButton->setToolTip("隨機播放");
    controlLayout->addWidget(shuffleButton);
    
    previousButton = new QPushButton("⏮", controlWidget);
    previousButton->setStyleSheet(buttonStyle);
    previousButton->setToolTip("上一首");
    controlLayout->addWidget(previousButton);
    
    playPauseButton = new QPushButton("▶", controlWidget);
    playPauseButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #1DB954;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 25px;"
        "   padding: 12px;"
        "   font-size: 18px;"
        "   min-width: 50px;"
        "   min-height: 50px;"
        "}"
        "QPushButton:hover { background-color: #1ED760; }"
        "QPushButton:pressed { background-color: #1AA34A; }"
        "QPushButton:disabled { background-color: #282828; color: #404040; }"
    );
    controlLayout->addWidget(playPauseButton);
    
    nextButton = new QPushButton("⏭", controlWidget);
    nextButton->setStyleSheet(buttonStyle);
    nextButton->setToolTip("下一首");
    controlLayout->addWidget(nextButton);
    
    repeatButton = new QPushButton("🔁", controlWidget);
    repeatButton->setStyleSheet(buttonStyle);
    repeatButton->setCheckable(true);
    repeatButton->setToolTip("循環播放");
    controlLayout->addWidget(repeatButton);
    
    controlLayout->addStretch();
    
    // 音量控制
    volumeLabel = new QLabel("🔊", controlWidget);
    volumeLabel->setStyleSheet("color: #B3B3B3; font-size: 16px; cursor: pointer;");
    volumeLabel->setToolTip("點擊以靜音/取消靜音");
    volumeLabel->installEventFilter(this);
    controlLayout->addWidget(volumeLabel);
    
    volumeSlider = new QSlider(Qt::Horizontal, controlWidget);
    volumeSlider->setRange(1, 100);
    volumeSlider->setValue(50);
    volumeSlider->setMaximumWidth(100);
    volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   border: none;"
        "   height: 4px;"
        "   background: #404040;"
        "   border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #1DB954;"
        "   border: none;"
        "   width: 10px;"
        "   height: 10px;"
        "   margin: -3px 0;"
        "   border-radius: 5px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "   background: #1ED760;"
        "}"
        "QSlider::sub-page:horizontal {"
        "   background: #1DB954;"
        "   border-radius: 2px;"
        "}"
    );
    controlLayout->addWidget(volumeSlider);
    
    // 加入播放清單功能
    addToPlaylistButton = new QPushButton("➕ 加入播放清單", controlWidget);
    addToPlaylistButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 10px 20px;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
        "QPushButton:disabled { background-color: #181818; color: #404040; }"
    );
    addToPlaylistButton->setEnabled(false);
    controlLayout->addWidget(addToPlaylistButton);
    
    targetPlaylistComboBox = new QComboBox(controlWidget);
    targetPlaylistComboBox->setStyleSheet(
        "QComboBox {"
        "   background-color: #282828;"
        "   border: 1px solid #404040;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "   color: #FFFFFF;"
        "   min-width: 100px;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #282828;"
        "   color: #FFFFFF;"
        "   selection-background-color: #1DB954;"
        "}"
    );
    targetPlaylistComboBox->setEnabled(false);
    controlLayout->addWidget(targetPlaylistComboBox);
    
    centerLayout->addWidget(controlWidget);
    
    contentSplitter->addWidget(centerPanel);
    
    // 設置分割器比例
    contentSplitter->setStretchFactor(0, 1);
    contentSplitter->setStretchFactor(1, 3);
    
    mainLayout->addWidget(contentSplitter, 1);
}

void Widget::createConnections()
{
    // 本地檔案載入
    connect(loadLocalFileButton, &QPushButton::clicked, this, &Widget::onLoadLocalFileClicked);
    
    // 播放控制按鈕
    connect(playPauseButton, &QPushButton::clicked, this, &Widget::onPlayPauseClicked);
    connect(previousButton, &QPushButton::clicked, this, &Widget::onPreviousClicked);
    connect(nextButton, &QPushButton::clicked, this, &Widget::onNextClicked);
    connect(shuffleButton, &QPushButton::clicked, this, &Widget::onShuffleClicked);
    connect(repeatButton, &QPushButton::clicked, this, &Widget::onRepeatClicked);
    
    // 播放清單管理
    connect(playlistWidget, &QListWidget::itemDoubleClicked, this, &Widget::onSongDoubleClicked);
    connect(playlistWidget, &QListWidget::itemSelectionChanged, this, &Widget::updateButtonStates);
    connect(playlistWidget, &QListWidget::customContextMenuRequested, this, &Widget::onPlaylistContextMenu);
    
    // 加入播放清單按鈕
    connect(addToPlaylistButton, &QPushButton::clicked, this, &Widget::onAddToPlaylistClicked);
    
    // 播放清單選擇
    connect(newPlaylistButton, &QPushButton::clicked, this, &Widget::onNewPlaylistClicked);
    connect(deletePlaylistButton, &QPushButton::clicked, this, &Widget::onDeletePlaylistClicked);
    connect(playlistComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Widget::onPlaylistChanged);
    
    // 媒體播放器
    connect(mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &Widget::onMediaPlayerStateChanged);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &Widget::onMediaPlayerPositionChanged);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &Widget::onMediaPlayerDurationChanged);
    
    // 進度條控制
    connect(progressSlider, &QSlider::sliderPressed, this, &Widget::onProgressSliderPressed);
    connect(progressSlider, &QSlider::sliderReleased, this, &Widget::onProgressSliderReleased);
    connect(progressSlider, &QSlider::sliderMoved, this, &Widget::onProgressSliderMoved);
    
    // 音量控制
    connect(volumeSlider, &QSlider::valueChanged, this, &Widget::onVolumeSliderChanged);
    
    // 播放清單拖放重排
    connect(playlistWidget->model(), &QAbstractItemModel::rowsMoved, 
            [this](const QModelIndex &, int, int, const QModelIndex &, int) {
                // 當項目被移動時，更新內部資料結構
                if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
                    Playlist& playlist = playlists[currentPlaylistIndex];
                    QList<MusicInfo> newSongs;
                    for (int i = 0; i < playlistWidget->count(); i++) {
                        QListWidgetItem* item = playlistWidget->item(i);
                        int oldIndex = item->data(Qt::UserRole).toInt();
                        if (oldIndex >= 0 && oldIndex < playlist.songs.size()) {
                            newSongs.append(playlist.songs[oldIndex]);
                        }
                    }
                    playlist.songs = newSongs;
                    // 重新分配索引
                    for (int i = 0; i < playlistWidget->count(); i++) {
                        playlistWidget->item(i)->setData(Qt::UserRole, i);
                    }
                    savePlaylistsToFile();
                }
            });
}

void Widget::onLoadLocalFileClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, 
        "選擇音樂檔案", 
        QDir::homePath(),
        "音樂檔案 (*.mp3 *.wav *.flac *.m4a *.ogg *.aac);;所有檔案 (*.*)");
    
    if (!filePath.isEmpty()) {
        // 創建音樂資訊
        MusicInfo music;
        music.filePath = filePath;
        
        // 從檔案名提取標題
        QFileInfo fileInfo(filePath);
        music.title = fileInfo.baseName();
        music.artist = "本地音樂";
        music.isFavorite = false;
        
        // 添加到當前播放清單
        if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
            // 檢查是否已存在
            bool alreadyExists = false;
            for (const MusicInfo& existingSong : playlists[currentPlaylistIndex].songs) {
                if (existingSong.filePath == filePath) {
                    alreadyExists = true;
                    break;
                }
            }
            
            if (!alreadyExists) {
                playlists[currentPlaylistIndex].songs.append(music);
                updatePlaylistDisplay();
                savePlaylistsToFile();
            }
            
            // 播放新添加的歌曲（或已存在的歌曲）
            int targetIndex = -1;
            for (int i = 0; i < playlists[currentPlaylistIndex].songs.size(); i++) {
                if (playlists[currentPlaylistIndex].songs[i].filePath == filePath) {
                    targetIndex = i;
                    break;
                }
            }
            
            if (targetIndex >= 0) {
                playMusic(targetIndex);
            }
        } else {
            // 如果沒有播放清單，直接播放
            playLocalFile(filePath);
        }
    }
}

void Widget::playLocalFile(const QString& filePath)
{
    // 停止當前播放
    mediaPlayer->stop();
    
    // 創建音樂資訊
    MusicInfo music;
    music.filePath = filePath;
    
    // 從檔案名提取標題
    QFileInfo fileInfo(filePath);
    music.title = fileInfo.baseName();
    music.artist = "本地音樂";
    music.isFavorite = false;
    
    // 檢查當前播放清單是否有效
    if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
        Playlist& playlist = playlists[currentPlaylistIndex];
        
        // 檢查檔案是否已存在於播放清單中
        int existingIndex = -1;
        for (int i = 0; i < playlist.songs.size(); i++) {
            if (playlist.songs[i].filePath == filePath) {
                existingIndex = i;
                break;
            }
        }
        
        if (existingIndex >= 0) {
            // 檔案已存在，直接播放
            currentMusicIndex = existingIndex;
            music = playlist.songs[existingIndex];
        } else {
            // 檔案不存在，加入播放清單
            playlist.songs.append(music);
            currentMusicIndex = playlist.songs.size() - 1;
            savePlaylistsToFile();
            updatePlaylistDisplay();
        }
    }
    
    // 設置媒體播放器
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    mediaPlayer->play();
    
    // 更新顯示
    updateMusicLabels(music);
    
    // 更新播放狀態
    isPlaying = true;
    playPauseButton->setText("⏸");
    
    updateButtonStates();
}

void Widget::onPlayPauseClicked()
{
    if (currentMusicIndex >= 0) {
        // 有正在播放的音樂
        if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
            const Playlist& playlist = playlists[currentPlaylistIndex];
            if (currentMusicIndex < playlist.songs.size()) {
                // 本地檔案，控制媒體播放器
                if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
                    mediaPlayer->pause();
                    isPlaying = false;
                    playPauseButton->setText("▶");
                } else {
                    mediaPlayer->play();
                    isPlaying = true;
                    playPauseButton->setText("⏸");
                }
            }
        }
    } else {
        // 沒有音樂，嘗試播放播放清單第一首
        if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
            Playlist& playlist = playlists[currentPlaylistIndex];
            if (!playlist.songs.isEmpty()) {
                playMusic(0);
            } else {
                QMessageBox::information(this, "提示", "播放清單是空的，請先載入音樂檔案。");
            }
        } else {
            QMessageBox::information(this, "提示", "請先選擇一個播放清單並載入音樂檔案。");
        }
    }
}

void Widget::onMediaPlayerStateChanged()
{
    // 當媒體播放器狀態改變時更新按鈕
    if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        isPlaying = true;
        playPauseButton->setText("⏸");
    } else if (mediaPlayer->playbackState() == QMediaPlayer::StoppedState) {
        isPlaying = false;
        playPauseButton->setText("▶");
        
        // 本地檔案播放結束，自動播放下一首（如果有）
        // 不在手動切換歌曲時觸發自動播放
        if (!isSwitchingSongs && currentMusicIndex >= 0 && currentPlaylistIndex >= 0 && 
            currentPlaylistIndex < playlists.size()) {
            const Playlist& playlist = playlists[currentPlaylistIndex];
            if (currentMusicIndex < playlist.songs.size()) {
                int nextIndex = getNextMusicIndex();
                if (nextIndex >= 0) {
                    playMusic(nextIndex);
                }
            }
        }
    }
}

void Widget::onMediaPlayerPositionChanged(qint64 position)
{
    // 更新進度條位置（當使用者沒有拖動時）
    if (!isProgressSliderPressed && mediaPlayer->duration() > 0) {
        progressSlider->setValue(position);
        
        // 更新當前時間顯示（mm:ss格式）
        int totalSeconds = position / 1000;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        currentTimeLabel->setText(QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0')));
    }
}

void Widget::onMediaPlayerDurationChanged(qint64 duration)
{
    // 設置進度條範圍
    progressSlider->setMaximum(duration);
    progressSlider->setEnabled(duration > 0);
    
    // 更新總時長顯示（mm:ss格式）
    int totalSeconds = duration / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    totalTimeLabel->setText(QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0')));
}

void Widget::onPreviousClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.songs.isEmpty()) return;
    
    if (isShuffleMode) {
        int newIndex = getRandomMusicIndex(true);
        if (newIndex >= 0) {
            playMusic(newIndex);
        }
    } else {
        int newIndex = currentMusicIndex - 1;
        if (newIndex < 0) {
            newIndex = playlist.songs.size() - 1;
        }
        playMusic(newIndex);
    }
}

void Widget::onNextClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.songs.isEmpty()) return;
    
    int newIndex = getNextMusicIndex();
    if (newIndex >= 0) {
        playMusic(newIndex);
    }
}

void Widget::onShuffleClicked()
{
    isShuffleMode = !isShuffleMode;
    shuffleButton->setChecked(isShuffleMode);
    
    if (isShuffleMode) {
        playedSongsInCurrentSession.clear();
        shuffleButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #1DB954;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #1ED760; }"
        );
    } else {
        shuffleButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #282828;"
            "   color: #FFFFFF;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #404040; }"
        );
    }
}

void Widget::onRepeatClicked()
{
    isRepeatMode = !isRepeatMode;
    repeatButton->setChecked(isRepeatMode);
    
    if (isRepeatMode) {
        repeatButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #1DB954;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #1ED760; }"
        );
    } else {
        repeatButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #282828;"
            "   color: #FFFFFF;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #404040; }"
        );
    }
}

void Widget::onSongDoubleClicked(QListWidgetItem* item)
{
    int index = playlistWidget->row(item);
    playMusic(index);
}

void Widget::onAddToPlaylistClicked()
{
    if (currentMusicIndex < 0 || currentPlaylistIndex < 0) return;
    if (currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& currentPlaylist = playlists[currentPlaylistIndex];
    if (currentMusicIndex >= currentPlaylist.songs.size()) return;
    
    MusicInfo& music = currentPlaylist.songs[currentMusicIndex];
    
    // 獲取目標播放清單索引
    int targetComboIndex = targetPlaylistComboBox->currentIndex();
    if (targetComboIndex < 0) return;
    
    // 找到目標播放清單的實際索引（跳過當前播放清單）
    int targetPlaylistIndex = -1;
    int comboCounter = 0;
    for (int i = 0; i < playlists.size(); i++) {
        if (i != currentPlaylistIndex) {
            if (comboCounter == targetComboIndex) {
                targetPlaylistIndex = i;
                break;
            }
            comboCounter++;
        }
    }
    
    if (targetPlaylistIndex < 0 || targetPlaylistIndex >= playlists.size()) return;
    
    Playlist& targetPlaylist = playlists[targetPlaylistIndex];
    
    // 檢查是否已存在於目標播放清單中
    bool alreadyExists = false;
    for (const MusicInfo& existingSong : targetPlaylist.songs) {
        if (existingSong.filePath == music.filePath) {
            alreadyExists = true;
            break;
        }
    }
    
    if (alreadyExists) {
        QMessageBox::information(this, "加入播放清單", 
            QString("「%1」已存在於播放清單「%2」中！")
            .arg(music.title)
            .arg(targetPlaylist.name));
    } else {
        // 加入目標播放清單
        targetPlaylist.songs.append(music);
        savePlaylistsToFile();
        QMessageBox::information(this, "加入播放清單", 
            QString("已將「%1」加入到播放清單「%2」！")
            .arg(music.title)
            .arg(targetPlaylist.name));
    }
}

void Widget::onNewPlaylistClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增播放清單", 
                                         "請輸入播放清單名稱:", 
                                         QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        // 檢查是否重複
        for (const Playlist& p : playlists) {
            if (p.name == name) {
                QMessageBox::warning(this, "新增播放清單", "播放清單名稱已存在！");
                return;
            }
        }
        
        Playlist newPlaylist;
        newPlaylist.name = name;
        playlists.append(newPlaylist);
        playlistComboBox->addItem(name);
        
        int newIndex = playlists.size() - 1;
        playlistComboBox->setCurrentIndex(newIndex);
        currentPlaylistIndex = newIndex;
        lastPlaylistName = name;
        updatePlaylistDisplay();
        updateTargetPlaylistComboBox();
        updateButtonStates();
    }
}

void Widget::onDeletePlaylistClicked()
{
    if (playlists.size() <= 1) {
        QMessageBox::warning(this, "無法刪除", "至少需要保留一個播放清單！");
        return;
    }
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    int ret = QMessageBox::question(this, "確認刪除", 
                                    QString("確定要刪除播放清單「%1」嗎？")
                                    .arg(playlists[currentPlaylistIndex].name),
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        currentMusicIndex = -1;
        isPlaying = false;
        playlists.removeAt(currentPlaylistIndex);
        playlistComboBox->removeItem(currentPlaylistIndex);
    }
}

void Widget::onPlaylistChanged(int index)
{
    if (index < 0 || index >= playlists.size()) return;
    
    currentPlaylistIndex = index;
    currentMusicIndex = -1;
    playedSongsInCurrentSession.clear();
    updatePlaylistDisplay();
    updateTargetPlaylistComboBox();
    updateButtonStates();
}

void Widget::updateTargetPlaylistComboBox()
{
    targetPlaylistComboBox->clear();
    
    // 添加所有播放清單，除了當前播放清單
    for (int i = 0; i < playlists.size(); i++) {
        if (i != currentPlaylistIndex) {
            targetPlaylistComboBox->addItem(playlists[i].name);
        }
    }
    
    // 如果有可選的播放清單，啟用按鈕和下拉選單
    bool hasTargetPlaylists = (targetPlaylistComboBox->count() > 0);
    targetPlaylistComboBox->setEnabled(hasTargetPlaylists && currentMusicIndex >= 0);
    addToPlaylistButton->setEnabled(hasTargetPlaylists && currentMusicIndex >= 0);
}

void Widget::updatePlaylistDisplay()
{
    playlistWidget->clear();
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    const Playlist& playlist = playlists[currentPlaylistIndex];
    for (int i = 0; i < playlist.songs.size(); i++) {
        const MusicInfo& music = playlist.songs[i];
        QString displayText = QString("%1\n   %2")
                                .arg(music.title)
                                .arg(music.artist);
        
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, i);
        
        // 高亮當前播放的音樂
        if (i == currentMusicIndex) {
            item->setBackground(QColor("#1DB954"));
            item->setForeground(QColor("#FFFFFF"));
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
        }
        
        playlistWidget->addItem(item);
    }
}

void Widget::playMusic(int index)
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (index < 0 || index >= playlist.songs.size()) return;
    
    // 停止標題恢復計時器，確保切換歌曲時立即顯示新歌曲標題
    titleRestoreTimer->stop();
    
    // 使用 RAII guard 確保 isSwitchingSongs 標誌總是被正確重置
    SongSwitchGuard guard(isSwitchingSongs);
    
    currentMusicIndex = index;
    const MusicInfo& music = playlist.songs[index];
    
    playedSongsInCurrentSession.insert(index);
    
    // 停止當前播放
    mediaPlayer->stop();
    
    // 播放本地檔案
    mediaPlayer->setSource(QUrl::fromLocalFile(music.filePath));
    mediaPlayer->play();
    
    isPlaying = true;
    playPauseButton->setText("⏸");
    
    // 更新顯示
    updateMusicLabels(music);
    
    updatePlaylistDisplay();
    updateButtonStates();
    
    playlistWidget->setCurrentRow(index);
}

void Widget::updateButtonStates()
{
    bool hasPlaylist = (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size());
    bool hasSongs = hasPlaylist && !playlists[currentPlaylistIndex].songs.isEmpty();
    int selectedRow = playlistWidget->currentRow();
    bool hasSelection = selectedRow >= 0;
    bool hasMediaPlaying = currentMusicIndex >= 0;
    
    playPauseButton->setEnabled(hasSongs || hasMediaPlaying);
    previousButton->setEnabled(hasSongs);
    nextButton->setEnabled(hasSongs);
    deletePlaylistButton->setEnabled(playlists.size() > 1);
    
    // 更新加入播放清單按鈕狀態
    bool hasTargetPlaylists = (targetPlaylistComboBox->count() > 0);
    addToPlaylistButton->setEnabled(hasMediaPlaying && hasTargetPlaylists);
    targetPlaylistComboBox->setEnabled(hasMediaPlaying && hasTargetPlaylists);
}

void Widget::savePlaylistsToFile()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }
    
    QString configFile = configDir + "/music_playlists.json";
    
    QJsonObject rootObj;
    QJsonArray playlistsArray;
    
    for (const Playlist& playlist : playlists) {
        QJsonObject playlistObj;
        playlistObj["name"] = playlist.name;
        
        QJsonArray songsArray;
        for (const MusicInfo& music : playlist.songs) {
            QJsonObject musicObj;
            musicObj["filePath"] = music.filePath;
            musicObj["title"] = music.title;
            musicObj["artist"] = music.artist;
            musicObj["isFavorite"] = music.isFavorite;
            songsArray.append(musicObj);
        }
        playlistObj["songs"] = songsArray;
        playlistsArray.append(playlistObj);
    }
    
    rootObj["playlists"] = playlistsArray;
    if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
        rootObj["lastPlaylist"] = playlists[currentPlaylistIndex].name;
    }
    
    QJsonDocument doc(rootObj);
    QFile file(configFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void Widget::loadPlaylistsFromFile()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString configFile = configDir + "/music_playlists.json";
    
    QFile file(configFile);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return;
    }
    
    QJsonObject rootObj = doc.object();
    lastPlaylistName = rootObj["lastPlaylist"].toString();
    
    QJsonArray playlistsArray = rootObj["playlists"].toArray();
    playlists.clear();
    
    for (const QJsonValue& value : playlistsArray) {
        QJsonObject playlistObj = value.toObject();
        Playlist playlist;
        playlist.name = playlistObj["name"].toString();
        
        QJsonArray songsArray = playlistObj["songs"].toArray();
        for (const QJsonValue& songValue : songsArray) {
            QJsonObject musicObj = songValue.toObject();
            MusicInfo music;
            music.filePath = musicObj["filePath"].toString();
            music.title = musicObj["title"].toString();
            music.artist = musicObj["artist"].toString();
            music.isFavorite = musicObj["isFavorite"].toBool();
            
            playlist.songs.append(music);
        }
        playlists.append(playlist);
    }
}

int Widget::getNextMusicIndex()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return -1;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.songs.isEmpty()) return -1;
    
    if (isShuffleMode) {
        return getRandomMusicIndex(true);
    } else {
        int newIndex = currentMusicIndex + 1;
        if (newIndex >= playlist.songs.size()) {
            if (isRepeatMode) {
                return 0;
            } else {
                return -1;
            }
        }
        return newIndex;
    }
}

int Widget::getRandomMusicIndex(bool excludeCurrent)
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return -1;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.songs.isEmpty()) return -1;
    
    if (playlist.songs.size() == 1) {
        if (excludeCurrent && currentMusicIndex == 0) {
            return -1;
        }
        return 0;
    }
    
    QList<int> unplayedSongs = getUnplayedMusicIndices(excludeCurrent);
    
    if (unplayedSongs.isEmpty() && isRepeatMode) {
        playedSongsInCurrentSession.clear();
        unplayedSongs = getUnplayedMusicIndices(excludeCurrent);
    }
    
    if (unplayedSongs.isEmpty()) {
        return -1;
    }
    
    int randomIndex = QRandomGenerator::global()->bounded(unplayedSongs.size());
    return unplayedSongs[randomIndex];
}

QList<int> Widget::getUnplayedMusicIndices(bool excludeCurrent)
{
    QList<int> unplayedSongs;
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) {
        return unplayedSongs;
    }
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    
    for (int i = 0; i < playlist.songs.size(); i++) {
        if (!playedSongsInCurrentSession.contains(i)) {
            if (!excludeCurrent || i != currentMusicIndex) {
                unplayedSongs.append(i);
            }
        }
    }
    
    return unplayedSongs;
}

void Widget::updateMusicLabels(const MusicInfo& music)
{
    musicTitleLabel->setText(music.title);
    artistLabel->setText(music.artist);
}

void Widget::restoreCurrentMusicTitle()
{
    if (currentMusicIndex >= 0 && currentPlaylistIndex >= 0 && 
        currentPlaylistIndex < playlists.size()) {
        const Playlist& playlist = playlists[currentPlaylistIndex];
        if (currentMusicIndex < playlist.songs.size()) {
            musicTitleLabel->setText(playlist.songs[currentMusicIndex].title);
        }
    }
}

void Widget::onProgressSliderPressed()
{
    isProgressSliderPressed = true;
}

void Widget::onProgressSliderReleased()
{
    isProgressSliderPressed = false;
    // 當使用者放開滑桿時，設置播放位置
    if (mediaPlayer->duration() > 0) {
        mediaPlayer->setPosition(progressSlider->value());
    }
}

void Widget::onProgressSliderMoved(int position)
{
    // 當使用者拖動滑桿時，更新時間顯示
    int totalSeconds = position / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    currentTimeLabel->setText(QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0')));
}

void Widget::onVolumeSliderChanged(int value)
{
    // 如果用戶手動調整音量滑桿，取消靜音狀態
    if (isMuted) {
        isMuted = false;
    }
    
    // 更新 previousVolume
    previousVolume = value;
    
    // 設置音量（value/100，範圍 1% 到 100%）
    qreal volume = value / 100.0;
    audioOutput->setVolume(volume);
    
    // 更新音量圖標
    updateVolumeIcon(value);
}

void Widget::updateVolumeIcon(int volume)
{
    if (volume == 0) {
        volumeLabel->setText("🔇");
    } else if (volume < 33) {
        volumeLabel->setText("🔈");
    } else if (volume < 66) {
        volumeLabel->setText("🔉");
    } else {
        volumeLabel->setText("🔊");
    }
}

void Widget::restoreCurrentMusicTitle()
{
    if (currentMusicIndex >= 0 && currentPlaylistIndex >= 0 && 
        currentPlaylistIndex < playlists.size()) {
        const Playlist& playlist = playlists[currentPlaylistIndex];
        if (currentMusicIndex < playlist.songs.size()) {
            musicTitleLabel->setText(playlist.songs[currentMusicIndex].title);
        }
    }
}

void Widget::onProgressSliderPressed()
{
    QListWidgetItem* item = playlistWidget->itemAt(pos);
    if (!item) return;
    
    int itemRow = playlistWidget->row(item);
    
    QMenu contextMenu(this);
    
    QAction* playAction = contextMenu.addAction("▶ 播放");
    QAction* deleteAction = contextMenu.addAction("🗑️ 從播放清單移除");
    
    QAction* selectedAction = contextMenu.exec(playlistWidget->mapToGlobal(pos));
    
    if (selectedAction == playAction) {
        playMusic(itemRow);
    } else if (selectedAction == deleteAction) {
        // 確保選中要刪除的項目
        playlistWidget->setCurrentRow(itemRow);
        onDeleteFromPlaylist();
    }
}

void Widget::onDeleteFromPlaylist()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    int selectedRow = playlistWidget->currentRow();
    if (selectedRow < 0) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (selectedRow >= playlist.songs.size()) return;
    
    // 如果刪除的是正在播放的歌曲，停止播放
    if (selectedRow == currentMusicIndex) {
        mediaPlayer->stop();
        currentMusicIndex = -1;
        musicTitleLabel->setText("選擇一首歌曲開始播放");
        artistLabel->setText("");
        isPlaying = false;
        playPauseButton->setText("▶");
    } else if (selectedRow < currentMusicIndex) {
        // 如果刪除的歌曲在當前播放歌曲之前，需要調整索引
        currentMusicIndex--;
    }
    
    // 從播放清單中移除
    playlist.songs.removeAt(selectedRow);
    
    // 更新顯示
    updatePlaylistDisplay();
    updateButtonStates();
    
    // 保存變更
    savePlaylistsToFile();
}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == volumeLabel && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            onVolumeLabelClicked();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void Widget::onVolumeLabelClicked()
{
    if (isMuted) {
        // 取消靜音，恢復之前的音量
        isMuted = false;
        // 確保至少有最小音量（避免從0恢復到0的情況）
        int restoreVolume = (previousVolume >= 1) ? previousVolume : 50;
        volumeSlider->setValue(restoreVolume);
        audioOutput->setVolume(restoreVolume / 100.0);
        updateVolumeIcon(restoreVolume);
    } else {
        // 靜音，保存當前音量
        previousVolume = volumeSlider->value();
        isMuted = true;
        // 直接設置音量為0，但不改變滑桿位置
        audioOutput->setVolume(0.0);
        updateVolumeIcon(0);
    }
}
