# Vibe CLI 整合實作總結

## 概述

本次實作根據需求，將 Qt 音樂播放器改為直接呼叫 Vibe CLI（Whisper 的命令列工具），取代原本的 Python 腳本方式。實現了自動語音轉錄功能，並支援可點擊的字幕時間戳。

## 實作的核心功能

### 1. 直接呼叫 Vibe CLI

按照需求規格實作：

```cpp
// Qt 使用 QProcess 直接啟動 Vibe
QProcess *process = new QProcess(this);
process->start("vibe", QStringList() << audioFilePath << "--output" << "output.srt");

// 連接完成信號
connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, [=](int, QProcess::ExitStatus){
    loadSrt("output.srt"); // 自動載入字幕檔
});
```

### 2. 自動化工作流程

播放音樂時自動執行以下流程：

1. **播放音樂** → 偵測到本地音樂檔案
2. **Qt 自動呼叫 Vibe** → 使用 QProcess 啟動 `vibe` 命令
3. **Vibe 生成字幕檔** → 在背景處理語音轉錄並生成 SRT 檔案
4. **Qt 自動載入顯示** → 轉錄完成後自動載入並解析 SRT 檔案，顯示可點擊的字幕

### 3. SRT 字幕支援

- **標準格式**：生成標準的 SubRip (SRT) 字幕檔案
- **自動解析**：智能解析 SRT 格式，處理序號、時間戳和多行字幕文字
- **可點擊時間戳**：點擊字幕中的時間範圍可跳轉到對應播放位置
- **檔案管理**：SRT 檔案與音訊檔案同名同位置，易於管理

### 4. 優雅的錯誤處理

- Vibe 未安裝時顯示友善的提示訊息
- SRT 檔案不存在或無法讀取時的錯誤處理
- 轉錄失敗時顯示退出碼和錯誤訊息

## 主要程式碼變更

### 1. `Widget::startWhisperTranscription()`

**檔案**: `widget.cpp`

```cpp
void Widget::startWhisperTranscription(const QString& audioFilePath)
{
    // 停止現有的 Whisper/Vibe 處理程序
    if (whisperProcess->state() != QProcess::NotRunning) {
        whisperProcess->kill();
        whisperProcess->waitForFinished();
    }
    
    // 清空字幕內容
    currentSubtitles = "";
    
    // 生成 SRT 輸出檔案路徑（跨平台）
    QFileInfo audioFileInfo(audioFilePath);
    QString baseName = audioFileInfo.completeBaseName();
    QDir outputDir(audioFileInfo.absolutePath());
    currentSrtFilePath = outputDir.filePath(baseName + ".srt");
    
    // 準備 Vibe CLI 參數
    QStringList arguments;
    arguments << audioFilePath << "--output" << currentSrtFilePath;
    
    // 啟動 Vibe 處理程序
    whisperProcess->start("vibe", arguments);
    
    // 錯誤處理...
}
```

### 2. `Widget::loadSrt()`

**檔案**: `widget.cpp`

新增函數用於載入和解析 SRT 字幕檔案：

```cpp
void Widget::loadSrt(const QString& srtFilePath)
{
    // 1. 檢查檔案是否存在
    // 2. 讀取 SRT 檔案內容
    // 3. 使用正則表達式解析 SRT 格式：
    //    - 序號行（純數字）
    //    - 時間戳行（HH:MM:SS,mmm --> HH:MM:SS,mmm）
    //    - 字幕文字行（可能多行）
    // 4. 將時間戳轉換為可點擊的 HTML 連結
    // 5. 更新顯示
}
```

### 3. `Widget::onWhisperFinished()`

**檔案**: `widget.cpp`

修改以自動載入生成的 SRT 檔案：

```cpp
void Widget::onWhisperFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0) {
        // 轉錄成功完成，載入 SRT 檔案
        loadSrt(currentSrtFilePath);
    } else {
        // 錯誤處理
    }
}
```

### 4. `Widget::updateSubtitleDisplay()`

**檔案**: `widget.cpp`

新增輔助函數以減少程式碼重複：

```cpp
void Widget::updateSubtitleDisplay()
{
    // 更新當前播放影片的字幕顯示
    // 檢查當前是否有本地音樂在播放
    // 呼叫 updateLocalMusicDisplay() 更新 UI
}
```

## 技術細節

### 跨平台相容性

- **路徑處理**：使用 `QDir::filePath()` 而非字串拼接，確保 Windows、Linux 和 macOS 相容
- **Qt 版本**：支援 Qt 5 和 Qt 6，使用條件編譯處理 API 差異
- **編碼處理**：Qt 5 使用 `setCodec()`，Qt 6 使用 `setEncoding()`

### 效能優化

- **正則表達式緩存**：將常用的正則表達式（`srtTimestampRegex`、`sequenceNumberRegex`）作為類別成員，在建構函數中初始化，避免重複編譯
- **匹配結果緩存**：在字幕解析迴圈中緩存正則表達式匹配結果，避免重複匹配
- **最小化記憶體配置**：使用 `QTextStream` 高效構建 HTML 字串

### 錯誤處理

1. **Vibe 未安裝**：
   - 檢查 `waitForStarted()` 返回值
   - 顯示安裝提示訊息

2. **SRT 檔案問題**：
   - 檔案不存在：顯示檔案路徑並提示
   - 無法開啟：顯示權限或檔案系統錯誤

3. **轉錄失敗**：
   - 捕捉退出碼
   - 讀取標準錯誤輸出
   - 顯示詳細錯誤訊息

## 使用方式

### 前置需求

安裝 Vibe CLI 工具（選擇以下任一方式）：

```bash
# 方法 1: whisper-ctranslate2 (推薦，速度快)
pip3 install whisper-ctranslate2
ln -s $(which whisper-ctranslate2) /usr/local/bin/vibe

# 方法 2: OpenAI Whisper 官方版本
pip3 install openai-whisper
# 建立 vibe 包裝腳本（參見 VIBE_SETUP.md）
```

### 使用流程

1. **啟動應用程式**：執行 Qt 音樂播放器
2. **載入音樂檔案**：點擊「📁 載入音樂檔案」按鈕
3. **自動轉錄**：播放時自動開始語音轉錄
4. **查看字幕**：轉錄完成後字幕自動顯示在播放器中
5. **點擊跳轉**：點擊字幕時間戳可跳到對應位置

### SRT 檔案位置

生成的 SRT 檔案會儲存在與音訊檔案相同的目錄中：

```
/path/to/music/song.mp3     → 音訊檔案
/path/to/music/song.srt     → 生成的字幕檔案
```

## 測試建議

### 基本功能測試

1. **正常流程測試**：
   ```
   - 載入一個 MP3 檔案
   - 確認 Vibe 自動啟動
   - 等待轉錄完成
   - 驗證字幕正確顯示
   - 測試時間戳點擊跳轉功能
   ```

2. **錯誤情況測試**：
   ```
   - Vibe 未安裝：應顯示友善的錯誤訊息
   - 權限問題：無法寫入 SRT 檔案
   - 檔案格式問題：不支援的音訊格式
   ```

3. **多檔案測試**：
   ```
   - 連續播放多個音訊檔案
   - 確認每個檔案都正確生成字幕
   - 檢查記憶體使用情況
   ```

### 效能測試

1. **不同檔案大小**：
   - 短音訊（< 1 分鐘）
   - 中等音訊（3-5 分鐘）
   - 長音訊（> 10 分鐘）

2. **不同音訊格式**：
   - MP3
   - WAV
   - FLAC
   - M4A
   - OGG

3. **不同語言**：
   - 中文
   - 英文
   - 混合語言

## 文件資源

### 專案文件

- **README.md**：專案總覽和使用說明
- **VIBE_SETUP.md**：Vibe CLI 詳細設定指南
- **QUICKSTART.md**：快速開始指南
- **IMPLEMENTATION_SUMMARY.md**（本檔案）：實作總結

### 程式碼位置

- **標頭檔**：`widget.h`
  - 新增成員變數：`currentSrtFilePath`、`srtTimestampRegex`、`sequenceNumberRegex`
  - 新增函數宣告：`loadSrt()`、`updateSubtitleDisplay()`

- **實作檔**：`widget.cpp`
  - 修改函數：`startWhisperTranscription()`、`onWhisperFinished()`、`onWhisperOutputReady()`
  - 新增函數：`loadSrt()`、`updateSubtitleDisplay()`

## 已知限制

1. **Vibe 相依性**：需要使用者自行安裝 Vibe CLI 工具
2. **轉錄時間**：大檔案可能需要較長時間處理
3. **語言支援**：依賴 Vibe 本身支援的語言
4. **模型選擇**：目前使用 Vibe 的預設模型設定

## 未來改進建議

1. **進度顯示**：顯示轉錄進度百分比
2. **模型選擇**：允許使用者選擇 Whisper 模型大小
3. **語言設定**：提供語言選擇介面
4. **批次處理**：支援批次轉錄多個檔案
5. **字幕編輯**：允許使用者編輯生成的字幕
6. **字幕匯出**：支援匯出為其他字幕格式（VTT、ASS 等）

## 總結

本次實作成功完成了需求中提出的功能：

✅ Qt 直接呼叫 Vibe CLI  
✅ 自動生成 SRT 字幕檔  
✅ 自動載入並顯示字幕  
✅ 可點擊時間戳跳轉  
✅ 跨平台相容（Windows、Linux、macOS）  
✅ Qt 5/6 相容  
✅ 效能優化  
✅ 完整文件  

實作遵循了 Qt 最佳實踐，具有良好的錯誤處理和使用者體驗。程式碼結構清晰，易於維護和擴展。
