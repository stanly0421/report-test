# 音樂播放器 (Music Player)

這是一個使用 Qt6 C++ 開發的音樂播放器，靈感來自 Spotify 的設計風格。支援本地音樂檔案播放和 YouTube 影片連結。

## 快速開始

1. **編譯**: 使用 CMake 或 qmake (詳見下方說明)
2. **使用**: 貼上 YouTube 連結或選擇本地音樂檔案開始播放

## 功能特色

### 1. 本地音樂播放
- 支援多種音樂格式：MP3、WAV、FLAC、M4A、OGG、AAC
- 使用檔案對話框選擇音樂檔案
- 內建音樂播放器，支援播放/暫停控制
- 自動顯示歌曲名稱（從檔案名提取）
- **即時字幕顯示**: 支援 Whisper 語音轉文字功能
- **可點擊時間戳**: 點擊字幕中的時間戳可直接跳轉到該位置播放

### 2. 播放清單管理
- 創建和管理多個播放清單
- 儲存本地音樂到播放清單（也支援 YouTube 連結的歷史數據）
- 支援從播放清單播放音樂
- 雙擊播放清單項目即可播放
- **拖放排序功能**: 可以用滑鼠拖放調整播放清單中歌曲的順序
- 點擊歌曲即可播放

### 3. 播放控制
- 播放/暫停按鈕（本地音樂支援實際控制）
- 上一首/下一首功能（完整支援）
- 隨機播放模式
- 循環播放模式
- 支援背景播放
- **播放進度條**: 使用 QSlider 控制和顯示播放進度，以 mm:ss 格式顯示當前時間和總時長
- **音量控制**: 使用 QSlider 調整音量大小（0-100%）
- **時間軸跳轉**: 透過點擊字幕時間戳或拖動進度條快速跳轉

### 4. Whisper 語音轉錄與字幕功能
- 使用 QProcess 執行 Python Whisper 腳本
- 即時讀取 Whisper 輸出並顯示字幕
- 自動更新字幕顯示
- 支援本地音樂檔案的語音轉文字
- **時間戳顯示**: 每段字幕都標示開始和結束時間
- **點擊跳轉**: 點擊任何時間戳即可跳轉到該時間點播放
- 字幕區域自動滾動顯示最新內容
- **手動載入字幕**: 可直接載入現有的 .srt 字幕檔案，無需等待自動轉錄

### 5. 播放清單管理功能
- 創建和管理多個播放清單
- 一鍵將正在播放的歌曲加入到選擇的播放清單
- 使用下拉選單選擇目標播放清單
- 快速組織和管理您的音樂收藏
- 自動檢查重複，避免同一首歌多次加入

### 6. Spotify 風格 UI
- 深色主題設計
- 現代化的介面佈局
- 綠色強調色 (#1DB954)
- 直覺的操作體驗
- 響應式設計

## 操作說明

### 播放本地音樂
1. 點擊頂部的「📁 載入音樂檔案」按鈕
2. 選擇音樂檔案
3. 音樂將自動開始播放
4. 使用播放控制按鈕控制播放
5. 字幕將自動顯示（需安裝 Whisper）
6. **手動載入字幕**: 如果已有字幕檔案，點擊「📝 載入字幕檔案」按鈕，選擇 .srt 檔案即可直接載入
7. **點擊時間戳跳轉**: 點擊字幕中顯示的時間戳（如 [0.00s - 5.23s]）可直接跳到該時間點

### 播放控制
- **▶ 播放/⏸ 暫停**: 開始或暫停播放當前音樂
- **⏮ 上一首**: 切換到上一首音樂
- **⏭ 下一首**: 切換到下一首音樂
- **🔀 隨機**: 啟用隨機播放模式
- **🔁 循環**: 啟用循環播放模式
- **進度條**: 拖動進度條可以跳轉到指定播放位置，顯示格式為 mm:ss
- **音量控制**: 使用滑桿調整音量（0-100%），圖標會根據音量變化
- **➕ 加入播放清單**: 選擇目標播放清單並將正在播放的歌曲加入

### 字幕功能使用說明
1. **自動顯示**: 播放本地音樂時，Whisper 會自動開始轉錄並顯示字幕
2. **手動載入**: 如果您已有 .srt 字幕檔案，點擊頂部的「📝 載入字幕檔案」按鈕即可直接載入
3. **時間戳顯示**: 每段字幕都會顯示時間範圍，例如：[0.00s - 5.23s]
4. **點擊跳轉**: 點擊任何時間戳即可跳轉到該時間點播放
5. **即時更新**: 字幕會在轉錄過程中即時更新
6. **自動滾動**: 字幕區域會自動滾動到最新內容

### 播放清單管理
- **➕ 新增**: 創建新的播放清單
- **🗑️ 刪除**: 刪除當前播放清單（至少保留一個）
- **雙擊音樂**: 立即播放該音樂
- **拖放排序**: 用滑鼠拖動歌曲可重新排列播放順序
- **加入播放清單**: 使用右下角的「➕ 加入播放清單」按鈕和下拉選單，將正在播放的歌曲加入到其他播放清單

### 語音轉錄 (Whisper/Vibe)
- 播放本地音樂時自動啟動轉錄
- **直接呼叫 Vibe CLI**: Qt 使用 QProcess 直接呼叫 `vibe` 命令列工具
- 自動生成 SRT 字幕檔案並載入顯示
- **點擊時間戳可跳轉**: 點擊字幕中的時間範圍可直接跳到該時間點播放
- 需要安裝 Vibe (Whisper CLI 工具)

## 編譯和運行

### 系統需求
- Qt 6.x 或更高版本（也支援 Qt 5.x）
- Qt Multimedia 模組
- C++17 編譯器
- 穩定的網路連線（播放 YouTube 影片時）
- Vibe CLI（選用，用於語音轉錄）

**注意**: 本專案不再需要 Qt WebEngineWidgets 模組，可以在任何標準 Qt 安裝中運行。

### 安裝依賴

#### Ubuntu/Debian
```bash
# 安裝 Qt 6 和必要模組
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-multimedia-dev

# 或安裝 Qt 5 (如果使用 Qt 5)
sudo apt-get install qtbase5-dev qtmultimedia5-dev

# 安裝 Vibe (Whisper CLI) (選用)
# 方法 1: 使用 whisper-ctranslate2 (推薦，更快速)
pip3 install whisper-ctranslate2
# 安裝後可使用 'whisper-ctranslate2' 命令，需建立 'vibe' 別名或符號連結

# 方法 2: 使用官方 openai-whisper
sudo apt-get install python3 python3-pip ffmpeg
pip3 install openai-whisper
# 可建立名為 'vibe' 的包裝腳本來呼叫 whisper CLI
```

#### Fedora/RHEL/CentOS
```bash
# Qt 6
sudo dnf install qt6-qtbase-devel qt6-qtmultimedia-devel

# 或 Qt 5
sudo dnf install qt5-qtbase-devel qt5-qtmultimedia-devel

# 安裝 Vibe (Whisper CLI) (選用)
sudo dnf install python3 python3-pip ffmpeg
pip3 install whisper-ctranslate2
# 或: pip3 install openai-whisper
```

#### macOS (使用 Homebrew)
```bash
brew install qt@6
# 或 brew install qt@5

# 安裝 Vibe (Whisper CLI) (選用)
brew install python ffmpeg
pip3 install whisper-ctranslate2
# 或: pip3 install openai-whisper
```

#### Windows
1. 下載並安裝 Qt from [qt.io](https://www.qt.io/download)
2. 在安裝時確保選擇以下組件：
   - Qt Multimedia
3. (選用) 安裝 Vibe (Whisper CLI)：
   - 下載 Python from [python.org](https://www.python.org)
   - 下載 FFmpeg from [ffmpeg.org](https://ffmpeg.org)
   - 執行: `pip install whisper-ctranslate2` 或 `pip install openai-whisper`
   - 確保 `vibe` 命令可在命令列中執行

### 編譯

#### 使用 qmake (傳統方式)
```bash
# Qt 6
qmake6 last-report.pro
make

# Qt 5
qmake last-report.pro
make
```

#### 使用 CMake (推薦)
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 運行
```bash
./last-report
```

## 技術實現

### 核心類別
- **QMediaPlayer**: 本地音樂播放器
- **QAudioOutput**: 音訊輸出控制
- **QTextBrowser**: HTML 內容顯示和連結開啟
- **QProcess**: 執行 Vibe CLI 語音轉錄工具
- **QFileDialog**: 檔案選擇對話框
- **QListWidget**: 顯示播放清單（支援拖放排序）
- **QComboBox**: 播放清單選擇器
- **QRegularExpression**: YouTube 連結解析和 SRT 字幕解析
- **QDesktopServices**: 在瀏覽器中開啟 YouTube 連結

### Vibe CLI 整合 (Whisper 語音轉錄)
- **直接呼叫 CLI**: Qt 使用 QProcess 直接啟動 `vibe` 命令列工具
- **命令格式**: `vibe <audioFilePath> --output <output.srt>`
- **自動流程**: 
  1. 播放音樂時，Qt 自動呼叫 Vibe CLI
  2. Vibe 在背景處理語音轉錄
  3. 生成 SRT 字幕檔案（與音訊檔案同名）
  4. Qt 監聽 QProcess 完成信號
  5. 自動載入並解析 SRT 檔案
  6. 顯示可點擊的字幕時間戳
- **SRT 解析**: 使用正則表達式解析 SRT 時間戳格式（HH:MM:SS,mmm）
- **互動功能**: 點擊字幕時間戳可跳轉到對應播放位置
- **錯誤處理**: 優雅處理 Vibe 未安裝或轉錄失敗的情況

### YouTube 連結處理
- 使用正則表達式解析 YouTube URL
- 支援多種 URL 格式
- 生成標準 YouTube 觀看連結
- 使用 QTextBrowser 顯示可點擊連結
- 點擊連結在系統預設瀏覽器中開啟

### 本地音樂播放
- 使用 Qt Multimedia 框架
- 支援多種音訊格式
- 即時播放控制
- 整合 Whisper 語音轉錄

### 資料持久化
- 使用 JSON 格式儲存播放清單
- 儲存位置：`QStandardPaths::AppDataLocation`
- 自動載入上次的播放清單
- 保存音樂資訊、檔案路徑和最愛狀態

## 界面預覽

播放器界面包含：
- **頂部工具欄**: Logo、本地音樂載入按鈕和字幕檔案載入按鈕
- **左側面板**: 播放清單管理（支援拖放排序，無編號顯示）
- **中央面板**: 
  - 音樂資訊顯示（標題和藝術家/頻道）
  - 本地音樂資訊顯示區域
  - **播放進度條**: 顯示當前播放位置（mm:ss 格式）和總時長
  - **字幕/轉錄文字顯示區域**: 帶可點擊時間戳，點擊可跳轉到該時間點
  - 播放控制按鈕（播放/暫停、上一首、下一首、隨機、循環）
  - **音量控制滑桿**: 調整音量大小（0-100%）
  - **加入播放清單功能**: 選擇目標播放清單並將正在播放的歌曲加入

## 注意事項

### YouTube 影片播放
- YouTube 影片連結已從主界面移除
- 歷史播放清單中的 YouTube 連結數據仍會保留（向下兼容）
- 如需播放 YouTube 影片，請在播放清單中雙擊 YouTube 項目（如果存在）

### 本地音樂播放
- 支援常見音訊格式
- 檔案路徑會被儲存在播放清單中
- 確保音樂檔案位置不會移動
- 支援 Vibe CLI 語音轉錄（需額外安裝）
- 播放進度條會即時更新顯示當前播放位置和總時長

### Vibe CLI 語音轉錄與字幕功能
- **使用 Vibe CLI**: 直接呼叫 `vibe` 命令列工具，無需 Python 腳本
- **安裝要求**: 需要安裝 Vibe (如 whisper-ctranslate2 或 openai-whisper 的 CLI 工具)
- **自動化流程**: 
  - 播放音樂 → Qt 自動呼叫 Vibe → Vibe 生成 SRT 字幕檔 → Qt 自動載入顯示
- **手動載入字幕**: 如果您已有 .srt 字幕檔案，可以點擊「📝 載入字幕檔案」按鈕直接載入，無需等待自動轉錄
- **SRT 格式**: 生成的字幕檔案為標準 SRT 格式，可用於其他播放器
- **檔案位置**: SRT 檔案與音訊檔案儲存在同一目錄，使用相同檔名
- **轉錄時間**: 轉錄過程可能需要較長時間（取決於音頻長度和使用的模型）
- **互動功能**: 點擊字幕中的時間戳（如 [0.00s - 5.23s]）可跳轉到該時間點
- **進度顯示**: 可顯示 Vibe CLI 的進度訊息（如果有輸出）

## 最新改進（v2.3）

- ✅ **手動載入字幕功能**: 新增「📝 載入字幕檔案」按鈕，可直接載入現有的 .srt 字幕檔案
- ✅ **彈性字幕來源**: 支援自動轉錄或手動載入字幕，滿足不同使用場景
- ✅ **改善用戶體驗**: 不再需要等待 Vibe 轉錄，可立即使用現有字幕檔案

## 之前的改進（v2.2）

- ✅ **直接整合 Vibe CLI**: 使用 QProcess 直接呼叫 `vibe` 命令，移除對 Python 腳本的依賴
- ✅ **SRT 字幕支援**: 自動生成和載入 SRT 格式字幕檔案
- ✅ **智能檔案管理**: SRT 檔案與音訊檔案同名同位置，便於管理
- ✅ **優雅的錯誤處理**: 當 Vibe 未安裝時顯示友善的提示訊息

## 之前的改進（v2.1）

- ✅ 將「加入最愛」按鈕改為「加入播放清單」功能
- ✅ 添加目標播放清單下拉選單，可選擇要加入的播放清單
- ✅ 播放清單切換功能正常運作，顯示所有加入的歌曲
- ✅ 支援拖放排序和雙擊播放功能
- ✅ 音量控制使用滑桿操作（v2.0 已實現）

## 之前的改進（v2.0）

- ✅ 添加播放進度條（QSlider），可拖動跳轉，顯示 mm:ss 格式時間
- ✅ 添加音量控制滑桿（QSlider），支援 0-100% 調整
- ✅ 移除播放清單項目的編號顯示，介面更簡潔
- ✅ 移除"檔案:"標籤，只顯示歌曲名稱
- ✅ 清理不需要的文檔和腳本檔案
- ✅ 改善使用者體驗和介面一致性

## 未來改進

- 支援播放清單分享功能
- 添加播放歷史記錄
- 支援深色/淺色主題切換
- 實作迷你播放器模式
- 支援更多音訊格式
- 添加等化器功能

## 疑難排解 (Troubleshooting)

### 常見問題

- **找不到 qmake**: 確認 Qt 已正確安裝並加入 PATH
- **CMake 找不到 Qt**: 設置 `CMAKE_PREFIX_PATH` 環境變數
- **缺少 DLL (Windows)**: 將 Qt bin 目錄加入 PATH 或複製所需 DLL
- **本地音樂無法播放**: 確認 Qt Multimedia 模組已正確安裝

祝您使用愉快！🎵