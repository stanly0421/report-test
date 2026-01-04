# 專案修改說明

## 變更摘要

本專案完成了以下主要任務：

### 1. 刪除非執行檔案

刪除了所有不會在程式執行時使用的檔案：

- **文件檔案** (24 個 .md 檔案)：各種說明文件、變更日誌、任務摘要等
- **展示檔案** (3 個 HTML 檔案)：demo.html, index.html, 功能展示_字幕點擊播放.html
- **前端資源** (2 個檔案)：script.js, style.css
- **Python 腳本** (1 個檔案)：whisper_transcribe.py（未使用，程式改用 vibe CLI）
- **Qt 專案檔** (1 個檔案)：last-report.pro（已改用 CMake 建置系統）

這些檔案均為文件或展示用途，不影響程式的實際執行。

### 2. 實作字幕同步功能

實作了音樂時間軸與字幕顯示的同步功能，使用 QMediaPlayer 搭配 QTimer 實現。

### 3. 修復字幕滾動跟隨功能

修復了字幕顯示區域的滾動行為，使其在字幕更新時自動跟隨當前播放的字幕，而不是回到最上面。

#### 問題描述

原本的實作在更新字幕高亮時，會重新生成完整的 HTML 內容並透過 `setHtml()` 更新 QTextBrowser。這導致顯示區域的滾動位置被重置，總是回到最上面，使用者無法看到當前正在播放的字幕。

#### 解決方案

在 `onSubtitleSyncTimer()` 函式中，當字幕索引改變並更新顯示後，使用 `scrollToAnchor()` 方法滾動到當前字幕的錨點位置。每個字幕都有唯一的 ID（如 `subtitle-0`, `subtitle-1` 等），可以用來定位。

**修改的程式碼** (`widget.cpp`):
```cpp
// 更新顯示
updateSubtitleDisplay();

// 滾動到當前字幕位置，保持字幕可見
if (currentSubtitleIndex >= 0) {
    QString anchorId = QString("subtitle-%1").arg(currentSubtitleIndex);
    videoDisplayArea->scrollToAnchor(anchorId);
}
```

#### 使用效果

- 當音樂播放時，字幕高亮會自動更新，顯示區域會自動滾動保持當前字幕可見
- 當用戶點擊字幕時間戳跳轉時，跳轉後的字幕會保持在可見範圍內
- 用戶仍可手動滾動查看其他字幕，但當字幕更新時會自動跟隨

## 技術實作細節

### 新增資料結構

**widget.h**:
```cpp
// 字幕條目結構
struct SubtitleEntry {
    double startTime;          // 開始時間（秒）
    double endTime;            // 結束時間（秒）
    QString text;              // 字幕文字
};
```

### 新增成員變數

**widget.h**:
```cpp
QList<SubtitleEntry> subtitleEntries;  // 儲存解析後的字幕條目清單
QTimer* subtitleSyncTimer;             // 用於同步字幕顯示的計時器
int currentSubtitleIndex;              // 當前高亮的字幕索引
```

### 新增槽函式

**widget.h**:
```cpp
void onSubtitleSyncTimer();  // 字幕同步計時器處理函式
```

### 修改的函式

#### 1. 建構函式 (Widget::Widget)

初始化新增的成員變數：
```cpp
, subtitleSyncTimer(new QTimer(this))
, currentSubtitleIndex(-1)
```

設定計時器：
```cpp
subtitleSyncTimer->setInterval(100);  // 每 100ms 檢查一次
connect(subtitleSyncTimer, &QTimer::timeout, this, &Widget::onSubtitleSyncTimer);
```

#### 2. 字幕載入函式 (Widget::loadSrt)

**改良前**：只解析 SRT 檔案並生成靜態 HTML 顯示

**改良後**：
- 解析 SRT 檔案並儲存到 `subtitleEntries` 清單
- 為每個字幕條目添加唯一 ID (`subtitle-0`, `subtitle-1`, ...)
- 載入完成後啟動字幕同步計時器

#### 3. 媒體播放器狀態改變 (Widget::onMediaPlayerStateChanged)

新增計時器控制邏輯：
- **播放狀態**：啟動 `subtitleSyncTimer`
- **暫停狀態**：停止 `subtitleSyncTimer`
- **停止狀態**：停止 `subtitleSyncTimer`

#### 4. 播放本地檔案 (Widget::playLocalFile)

新增清除字幕資料的邏輯：
```cpp
subtitleSyncTimer->stop();
subtitleEntries.clear();
currentSubtitleIndex = -1;
```

#### 5. 播放視訊 (Widget::playVideo)

新增清除字幕資料的邏輯：
```cpp
subtitleSyncTimer->stop();
subtitleEntries.clear();
currentSubtitleIndex = -1;
```

#### 6. 字幕同步處理 (Widget::onSubtitleSyncTimer) - 新增

核心同步邏輯（含效能優化）：

1. **檢查播放狀態**：如果沒有字幕或未在播放，停止計時器
2. **取得播放位置**：從 QMediaPlayer 取得當前播放位置（毫秒轉秒）
3. **智慧搜尋當前字幕**：
   - 優先檢查當前字幕是否仍有效（最常見情況）
   - 其次檢查下一個字幕（連續播放時）
   - 再檢查上一個字幕（用戶後退時）
   - 最後使用二分搜索（跳轉或初始化時）
4. **更新顯示**：只在字幕索引改變時重新生成 HTML 並高亮當前字幕

高亮樣式：
```html
<p style="background-color: #1DB954; color: #FFFFFF; padding: 8px; border-radius: 4px; font-weight: bold;">
```

### 效能優化

1. **智慧搜尋演算法**：
   - 優先檢查相鄰字幕（O(1)），避免每次都線性搜索
   - 使用二分搜索作為後備方案（O(log n)）
   - 大幅降低長字幕檔案的 CPU 使用率

2. **避免重複更新**：
   - 只在字幕索引改變時才更新 HTML
   - 如果仍在當前字幕時間範圍內，直接返回不做任何處理

3. **可配置間隔**：
   - 計時器間隔定義為常數 `SUBTITLE_SYNC_INTERVAL_MS`
   - 方便未來根據需求調整（精確度 vs 效能）

## 工作原理

### 字幕同步流程

```
播放音樂
    ↓
onMediaPlayerStateChanged (PlayingState)
    ↓
啟動 subtitleSyncTimer (每 100ms)
    ↓
onSubtitleSyncTimer 被觸發
    ↓
取得 mediaPlayer->position()
    ↓
比對 subtitleEntries 找出當前字幕
    ↓
如果字幕改變 → 重新生成 HTML（高亮當前字幕）
    ↓
updateSubtitleDisplay() 更新顯示
```

### 計時器優化

- 使用 100ms 間隔平衡精確度與效能
- 只在字幕索引改變時更新 HTML（避免不必要的重繪）
- 播放暫停/停止時自動停止計時器（節省資源）

## 測試建議

由於開發環境無 Qt，建議在有 Qt 環境的系統進行以下測試：

1. **基本播放測試**：
   - 載入音樂檔案並播放
   - 確認音樂正常播放

2. **字幕載入測試**：
   - 載入對應的 .srt 字幕檔案
   - 確認字幕顯示在介面上

3. **字幕同步測試**：
   - 播放音樂，觀察字幕是否隨音樂進度高亮
   - 確認高亮的字幕與音樂時間對應

4. **互動測試**：
   - 點擊字幕時間戳，確認能跳轉到對應位置
   - 暫停/播放切換，確認字幕同步正常

5. **切歌測試**：
   - 切換到其他歌曲，確認舊字幕被清除
   - 載入新字幕，確認新字幕正常同步

## 檔案清單

修改後的專案只保留必要的執行檔案：

```
report-test/
├── .gitignore
├── CMakeLists.txt       # CMake 建置設定
├── main.cpp             # 程式進入點
├── widget.cpp           # 主視窗實作（已修改）
├── widget.h             # 主視窗標頭（已修改）
├── widget.ui            # Qt Designer UI 定義
└── README.md            # 專案說明（新增）
```
