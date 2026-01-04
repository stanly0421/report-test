# 如何添加音樂文件

由於音樂文件較大，本專案不包含示範音樂文件。您可以按照以下步驟添加您自己的音樂：

## 方法 1: 使用您自己的音樂文件

1. 準備一個 MP3 格式的音樂文件
2. 將文件重命名為 `sample-audio.mp3`
3. 將文件放在專案根目錄（與 index.html 同一目錄）
4. 在瀏覽器中打開 index.html

## 方法 2: 使用不同名稱的音樂文件

1. 將您的音樂文件放在專案目錄
2. 編輯 `index.html` 文件，找到這一行：
   ```html
   <source src="sample-audio.mp3" type="audio/mpeg">
   ```
3. 將 `sample-audio.mp3` 改為您的文件名，例如：
   ```html
   <source src="my-song.mp3" type="audio/mpeg">
   ```

## 方法 3: 使用線上音樂（需要 CORS 支援）

如果您有線上音樂 URL，可以直接使用：
```html
<source src="https://example.com/your-music.mp3" type="audio/mpeg">
```

注意：某些網站可能不允許跨域訪問音樂文件。

## 自定義歌詞時間軸

在 `script.js` 文件中，修改 `lyricsData` 陣列：

```javascript
const lyricsData = [
    { time: 0, text: "第一句歌詞" },
    { time: 5.2, text: "第二句歌詞（5.2秒時出現）" },
    { time: 10.5, text: "第三句歌詞（10.5秒時出現）" },
    // 繼續添加...
];
```

### 如何確定時間點

1. 使用音樂編輯軟體（如 Audacity）查看波形
2. 在播放器中反覆播放，記錄每句歌詞出現的時間
3. 使用專業的歌詞編輯工具（如 LRC Editor）

## 支援的音頻格式

- MP3 (推薦，相容性最好)
- WAV (檔案較大)
- OGG (較小的檔案大小)
- M4A (Apple 設備友好)

## 測試功能

即使沒有音樂文件，您仍然可以：
- 查看介面設計
- 看到所有歌詞
- 點擊歌詞（雖然沒有聲音）
- 測試介面的響應性

添加音樂文件後，您將能體驗完整的即時字幕同步功能！
