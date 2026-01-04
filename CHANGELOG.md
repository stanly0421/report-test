# 更新日誌 (Changelog)

## [v2.3] - 2026-01-04

### 新增功能 ✨
- **手動載入字幕功能**: 新增「📝 載入字幕檔案」按鈕
  - 支援直接載入現有的 .srt 字幕檔案
  - 無需等待 Vibe 自動轉錄，立即使用現有字幕
  - 按鈕位於頂部工具欄，音樂載入按鈕旁邊
  - 只在播放音樂時才啟用該按鈕
  
### 改進功能 🎯
- **彈性字幕來源**: 
  - 支援自動轉錄（Vibe CLI）
  - 支援手動載入（直接選擇 .srt 檔案）
  - 兩種方式均使用相同的字幕顯示和互動功能
  
- **改善用戶體驗**:
  - 解決用戶「沒有字幕可以上傳」的問題
  - 提供更快速的字幕載入方式
  - 適合已有字幕檔案的使用場景

### 技術實現 🔧
- 新增成員變數：
  - `loadSubtitleButton`: QPushButton* - 載入字幕檔案按鈕

- 新增成員函數：
  - `onLoadSubtitleFileClicked()`: 處理載入字幕檔案按鈕點擊

- 更新函數：
  - `setupUI()`: 添加載入字幕按鈕到頂部工具欄
  - `createConnections()`: 連接載入字幕按鈕信號
  - `updateButtonStates()`: 根據播放狀態啟用/停用載入字幕按鈕

### 文檔更新 📚
- 更新 README.md：
  - 新增「最新改進（v2.3）」章節
  - 更新「功能特色」第4點，增加手動載入字幕說明
  - 更新「播放本地音樂」操作說明
  - 更新「字幕功能使用說明」，新增手動載入步驟
  - 更新「界面預覽」描述新的按鈕
  - 將 v2.2 改進移至「之前的改進」章節

## [v2.1] - 2026-01-03

### 新增功能 ✨
- **加入播放清單功能**: 將「加入最愛」改為更靈活的「加入播放清單」功能
  - 新增目標播放清單下拉選單（QComboBox）
  - 按鈕文字改為「➕ 加入播放清單」
  - 可選擇要加入的目標播放清單
  - 自動過濾當前播放清單
  - 自動檢查重複，避免同一首歌多次加入
  - 加入成功後顯示確認訊息

### 改進功能 🎯
- **播放清單管理**: 
  - 移除右鍵選單中的「加入最愛」選項
  - 改用統一的「加入播放清單」按鈕和下拉選單
  - 簡化播放清單操作流程
  
- **按鈕狀態管理**:
  - 自動啟用/停用加入播放清單按鈕
  - 根據是否有其他可選播放清單動態調整
  - 只在正在播放歌曲時才啟用功能

### 技術實現 🔧
- 新增成員變數：
  - `addToPlaylistButton`: QPushButton* - 加入播放清單按鈕
  - `targetPlaylistComboBox`: QComboBox* - 目標播放清單選擇器

- 移除成員變數：
  - `toggleFavoriteButton`: QPushButton* - 舊的最愛按鈕

- 新增成員函數：
  - `updateTargetPlaylistComboBox()`: 更新目標播放清單下拉選單
  - `onAddToPlaylistClicked()`: 處理加入播放清單按鈕點擊

- 移除成員函數：
  - `onToggleFavoriteClicked()`: 舊的最愛按鈕處理
  - `toggleFavoriteForVideo()`: 舊的最愛切換邏輯

- 更新函數：
  - `setupUI()`: 添加新按鈕和下拉選單控件
  - `createConnections()`: 連接新按鈕信號
  - `onPlaylistChanged()`: 調用 `updateTargetPlaylistComboBox()`
  - `onNewPlaylistClicked()`: 創建新播放清單後更新下拉選單
  - `updateButtonStates()`: 更新新按鈕的啟用狀態
  - `playVideo()`: 移除舊的最愛按鈕文字更新邏輯
  - `onPlaylistContextMenu()`: 移除右鍵選單的加入最愛選項
  - Widget 建構函數: 初始化時調用 `updateTargetPlaylistComboBox()`

### 文檔更新 📚
- 更新 README.md：
  - 新增「最新改進（v2.1）」章節
  - 更新「功能特色」第5點說明
  - 更新「操作說明」的播放控制和播放清單管理說明
  - 更新「界面預覽」描述新的控制元件
  - 將 v2.0 改進移至「之前的改進」章節

## [v2.0] - 2026-01-03

### 新增功能 ✨
- **播放進度條**: 添加 QSlider 控制播放進度
  - 即時顯示當前播放位置和總時長（mm:ss 格式）
  - 可拖動進度條跳轉到指定位置
  - 拖動時即時更新時間顯示

- **音量控制**: 添加 QSlider 調整音量
  - 支援 0-100% 音量調整
  - 音量圖標根據音量大小自動切換（🔇/🔈/🔉/🔊）
  - 即時調整，無需確認

### 改進功能 🎯
- **智能最愛按鈕**: 根據當前播放清單自動切換按鈕文字
  - 在「我的最愛」播放清單中顯示「💔 移除最愛」
  - 在其他播放清單中顯示「❤️ 加入最愛」
  - 根據歌曲是否已在最愛中智能顯示

- **簡潔播放清單顯示**: 
  - 移除播放清單項目的編號（如 "1. ", "2. " 等）
  - 只顯示歌曲名稱和藝術家/頻道
  - 介面更加簡潔美觀

- **精簡音樂資訊顯示**:
  - 移除"檔案:"標籤
  - 只顯示歌曲標題，減少視覺雜訊

### 程式碼清理 🧹
- 移除不需要的文檔檔案：
  - CHANGES.md
  - CHANGES_SUMMARY.md
  - CHANGES_SUMMARY_FINAL.md
  - IMPLEMENTATION_NOTES.md
  - MIGRATION_NOTES.md
  - PROJECT_STATUS.txt
  - QUICK_REFERENCE.md
  - SOLUTION_SUMMARY.txt
  - SOLUTION_SUMMARY_NEW.md
  - TESTING_GUIDE.md
  - UI_DESIGN.md
  - USER_GUIDE.md
  - VERIFICATION.md
  - WHISPER_GUIDE.md

- 移除不需要的腳本檔案：
  - check_webengine.bat
  - check_webengine.sh

### 技術實現 🔧
- 新增成員變數：
  - `progressSlider`: QSlider* - 播放進度條
  - `currentTimeLabel`: QLabel* - 當前時間標籤
  - `totalTimeLabel`: QLabel* - 總時長標籤
  - `volumeSlider`: QSlider* - 音量控制滑桿
  - `volumeLabel`: QLabel* - 音量圖標標籤
  - `isProgressSliderPressed`: bool - 追蹤進度條拖動狀態

- 新增槽函數：
  - `onProgressSliderPressed()`: 處理進度條按下事件
  - `onProgressSliderReleased()`: 處理進度條釋放事件
  - `onProgressSliderMoved(int)`: 處理進度條拖動事件
  - `onVolumeSliderChanged(int)`: 處理音量變化事件

- 更新槽函數：
  - `onMediaPlayerPositionChanged()`: 更新進度條和時間顯示
  - `onMediaPlayerDurationChanged()`: 設置進度條範圍和總時長
  - `onPlaylistChanged()`: 更新最愛按鈕文字
  - `playVideo()`: 智能設置最愛按鈕文字
  - `toggleFavoriteForVideo()`: 根據播放清單上下文更新按鈕
  - `updatePlaylistDisplay()`: 移除編號顯示
  - `generateLocalMusicHTML()`: 移除檔案標籤
  - `updateLocalMusicDisplay()`: 簡化顯示內容

### 文檔更新 📚
- 更新 README.md：
  - 新增「最新改進（v2.0）」章節
  - 更新「功能特色」說明
  - 更新「操作說明」增加進度條和音量控制說明
  - 更新「界面預覽」描述新增的控制元件
  - 移除「未來改進」中已完成的項目

## [v1.0] - 之前版本

### 主要功能
- 本地音樂播放支援
- YouTube 影片連結支援（歷史兼容）
- 播放清單管理
- Whisper 語音轉錄與字幕功能
- 拖放排序功能
- 我的最愛功能
- Spotify 風格 UI 設計
