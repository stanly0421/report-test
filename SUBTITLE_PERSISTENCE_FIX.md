# 字幕持久化修復 (Subtitle Persistence Fix)

## 問題描述 (Problem Description)

之前，當使用「載入音樂檔案」按鈕載入音樂後上傳字幕檔案，字幕在關閉檔案或重啟應用程式後會被清除。

Previously, when loading music via the "Load Music File" button and then uploading a subtitle file, the subtitles would be lost after closing the file or restarting the application.

## 根本原因 (Root Cause)

當透過「載入音樂檔案」按鈕載入音樂時：
1. `playLocalFile()` 函數會將 `currentVideoIndex` 設為 -1（表示不屬於播放清單）
2. 當使用者上傳字幕時，`onLoadSubtitleFileClicked()` 函數中的保存邏輯會檢查 `currentVideoIndex >= 0`
3. 由於 `currentVideoIndex` 為 -1，字幕路徑不會被保存到播放清單資料中
4. 關閉檔案或重啟應用程式後，字幕資訊丟失

When loading music via the "Load Music File" button:
1. The `playLocalFile()` function sets `currentVideoIndex` to -1 (indicating not part of a playlist)
2. When users upload subtitles, the save logic in `onLoadSubtitleFileClicked()` checks for `currentVideoIndex >= 0`
3. Since `currentVideoIndex` is -1, the subtitle path is not saved to the playlist data
4. After closing the file or restarting the app, subtitle information is lost

## 解決方案 (Solution)

修改 `playLocalFile()` 函數，使其：
1. 檢查音樂檔案是否已存在於當前播放清單中
2. 如果不存在，自動將其加入當前播放清單
3. 設定 `currentVideoIndex` 為正確的索引值
4. 保存播放清單資料以持久化變更
5. 在播放時檢查是否有已保存的字幕，如果有則自動載入

Modified the `playLocalFile()` function to:
1. Check if the music file already exists in the current playlist
2. If not, automatically add it to the current playlist
3. Set `currentVideoIndex` to the correct index value
4. Save the playlist data to persist changes
5. When playing, check for saved subtitles and automatically load them if they exist

## 程式碼變更 (Code Changes)

### 修改檔案 (Modified File)
- `widget.cpp` - `playLocalFile()` 函數

### 關鍵變更 (Key Changes)

```cpp
// 檢查當前播放清單是否有效
if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
    Playlist& playlist = playlists[currentPlaylistIndex];
    
    // 檢查檔案是否已存在於播放清單中
    int existingIndex = -1;
    for (int i = 0; i < playlist.videos.size(); i++) {
        if (playlist.videos[i].filePath == filePath) {
            existingIndex = i;
            break;
        }
    }
    
    if (existingIndex >= 0) {
        // 檔案已存在，直接播放並使用已保存的資訊
        currentVideoIndex = existingIndex;
        video = playlist.videos[existingIndex];
    } else {
        // 檔案不存在，加入播放清單
        playlist.videos.append(video);
        currentVideoIndex = playlist.videos.size() - 1;
        savePlaylistsToFile();
        updatePlaylistDisplay();
    }
}

// 檢查是否有保存的字幕
if (!video.subtitlePath.isEmpty() && QFile::exists(video.subtitlePath)) {
    // 自動載入已保存的字幕
    loadSrt(video.subtitlePath);
} else {
    // 啟動 Whisper 轉錄
    startWhisperTranscription(filePath);
}
```

## 使用流程 (Usage Flow)

### 第一次使用 (First Time Use)
1. 開啟應用程式
2. 點擊「📁 載入音樂檔案」按鈕
3. 選擇音樂檔案 → **音樂自動加入當前播放清單**
4. 點擊「📝 載入字幕檔案」按鈕
5. 選擇字幕檔案 → **字幕路徑保存到播放清單資料**

### 再次使用 (Subsequent Use)
1. 關閉應用程式或切換到其他音樂
2. 重新開啟應用程式
3. 再次點擊「📁 載入音樂檔案」並選擇相同的音樂檔案
4. **應用程式自動偵測到已保存的字幕並載入** ✅

或者：
1. 在播放清單中雙擊該音樂
2. **字幕自動載入** ✅

## 測試步驟 (Testing Steps)

### 測試 1: 新音樂檔案 + 字幕上傳
1. 編譯並執行應用程式
2. 載入一個新的音樂檔案
3. 驗證音樂出現在當前播放清單中
4. 上傳一個字幕檔案
5. 驗證字幕正確顯示
6. 關閉應用程式
7. 重新開啟應用程式
8. 再次載入相同的音樂檔案
9. **驗證字幕自動載入** ✅

### 測試 2: 已存在於播放清單的音樂
1. 載入一個已經在播放清單中的音樂檔案
2. 驗證 `currentVideoIndex` 設為正確的索引
3. 驗證已保存的字幕（如果有）自動載入
4. 不會在播放清單中創建重複項目

### 測試 3: 播放清單中播放
1. 在播放清單中雙擊一個已有字幕的音樂
2. 驗證字幕自動載入（此功能原本就有）

## 優點 (Benefits)

1. ✅ **字幕持久化**: 上傳的字幕會永久保存，即使關閉檔案也不會丟失
2. ✅ **自動載入**: 重新播放同一音樂時，字幕自動載入，無需重新上傳
3. ✅ **統一管理**: 所有透過「載入音樂檔案」載入的音樂都會加入播放清單，便於管理
4. ✅ **避免重複**: 同一檔案不會在播放清單中重複出現
5. ✅ **向下相容**: 不影響現有的播放清單功能和字幕載入邏輯

## 注意事項 (Notes)

- 必須有有效的當前播放清單（`currentPlaylistIndex >= 0`）才能保存字幕
- 如果沒有播放清單，建議先創建一個播放清單
- 字幕檔案必須存在於磁碟上才能自動載入
- 播放清單資料保存在 `QStandardPaths::AppDataLocation/youtube_playlists.json`

## 相關檔案 (Related Files)

- `widget.cpp` - 主要邏輯實作
- `widget.h` - VideoInfo 結構定義（包含 subtitlePath 欄位）
- `youtube_playlists.json` - 播放清單資料儲存檔案

## 版本資訊 (Version Info)

- 修復日期: 2026-01-04
- 修改檔案: `widget.cpp`
- 受影響函數: `playLocalFile()`
