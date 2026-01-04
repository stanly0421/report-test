# Vibe CLI 設定指南

本文件說明如何設定 Vibe CLI 以便與音樂播放器整合使用語音轉錄功能。

## 什麼是 Vibe？

Vibe 是 Whisper 的命令列介面工具，用於將音訊檔案轉錄為文字並生成字幕檔案。

## 安裝選項

### 選項 1: whisper-ctranslate2 (推薦，速度更快)

```bash
# 安裝 whisper-ctranslate2
pip3 install whisper-ctranslate2

# 確認安裝
whisper-ctranslate2 --help
```

如果您想使用 `vibe` 作為命令名稱，可以建立別名或符號連結：

```bash
# Linux/macOS: 建立符號連結
sudo ln -s $(which whisper-ctranslate2) /usr/local/bin/vibe

# 或在 ~/.bashrc 或 ~/.zshrc 中添加別名
echo "alias vibe='whisper-ctranslate2'" >> ~/.bashrc
source ~/.bashrc
```

### 選項 2: OpenAI Whisper 官方版本

```bash
# 安裝依賴
# Ubuntu/Debian
sudo apt-get install python3 python3-pip ffmpeg

# macOS
brew install python ffmpeg

# 安裝 whisper
pip3 install openai-whisper

# 確認安裝
whisper --help
```

建立 `vibe` 包裝腳本：

```bash
# 建立包裝腳本
cat > /usr/local/bin/vibe << 'EOF'
#!/bin/bash
# Vibe wrapper for OpenAI Whisper
whisper "$@"
EOF

# 設定執行權限
chmod +x /usr/local/bin/vibe
```

### 選項 3: 建立自訂 vibe 腳本

如果您想要更多控制或自訂選項，可以建立自己的 `vibe` 腳本：

```bash
cat > /usr/local/bin/vibe << 'EOF'
#!/bin/bash
# 自訂 Vibe 腳本用於 Whisper 轉錄

# 預設使用 base 模型
MODEL="base"
LANGUAGE="zh"

# 解析參數
INPUT_FILE=""
OUTPUT_FILE=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --output)
            OUTPUT_FILE="$2"
            shift 2
            ;;
        --model)
            MODEL="$2"
            shift 2
            ;;
        --language)
            LANGUAGE="$2"
            shift 2
            ;;
        *)
            if [ -z "$INPUT_FILE" ]; then
                INPUT_FILE="$1"
            fi
            shift
            ;;
    esac
done

# 檢查輸入檔案
if [ -z "$INPUT_FILE" ]; then
    echo "錯誤: 未指定輸入檔案"
    exit 1
fi

# 如果未指定輸出檔案，使用預設名稱
if [ -z "$OUTPUT_FILE" ]; then
    OUTPUT_FILE="${INPUT_FILE%.*}.srt"
fi

# 呼叫 whisper
echo "正在轉錄: $INPUT_FILE"
echo "輸出檔案: $OUTPUT_FILE"
echo "模型: $MODEL, 語言: $LANGUAGE"

whisper "$INPUT_FILE" \
    --model "$MODEL" \
    --language "$LANGUAGE" \
    --output_format srt \
    --output_dir "$(dirname "$OUTPUT_FILE")"

# 重新命名輸出檔案
WHISPER_OUTPUT="$(dirname "$OUTPUT_FILE")/$(basename "${INPUT_FILE%.*}").srt"
if [ "$WHISPER_OUTPUT" != "$OUTPUT_FILE" ] && [ -f "$WHISPER_OUTPUT" ]; then
    mv "$WHISPER_OUTPUT" "$OUTPUT_FILE"
fi

echo "轉錄完成！"
EOF

chmod +x /usr/local/bin/vibe
```

## 測試安裝

測試 `vibe` 命令是否正常運作：

```bash
# 確認 vibe 可執行
which vibe

# 測試轉錄（使用測試音訊檔案）
vibe test.mp3 --output test.srt

# 檢查生成的 SRT 檔案
cat test.srt
```

## Qt 音樂播放器整合

當您正確安裝 `vibe` 後，Qt 音樂播放器會：

1. **自動偵測**: 播放本地音樂時自動嘗試呼叫 `vibe`
2. **背景轉錄**: Vibe 在背景處理語音轉文字
3. **生成字幕**: 建立與音訊檔案同名的 `.srt` 檔案
4. **自動載入**: 轉錄完成後自動載入並顯示字幕
5. **可點擊時間戳**: 點擊字幕時間戳跳轉到對應位置

### 呼叫範例

播放器會使用以下命令呼叫 Vibe：

```bash
vibe /path/to/audio/file.mp3 --output /path/to/audio/file.srt
```

### SRT 格式範例

生成的 SRT 檔案格式：

```
1
00:00:00,000 --> 00:00:05,230
這是第一段字幕文字

2
00:00:05,230 --> 00:00:10,000
這是第二段字幕文字

3
00:00:10,000 --> 00:00:15,500
這是第三段字幕文字
```

## 故障排除

### 問題 1: 找不到 vibe 命令

**解決方法**:
```bash
# 檢查 vibe 是否在 PATH 中
which vibe

# 如果找不到，檢查 whisper 安裝
which whisper
which whisper-ctranslate2

# 建立符號連結或別名（見上方安裝選項）
```

### 問題 2: 權限被拒絕

**解決方法**:
```bash
# 給予 vibe 腳本執行權限
chmod +x /usr/local/bin/vibe

# 或者使用 sudo
sudo chmod +x /usr/local/bin/vibe
```

### 問題 3: 轉錄失敗或無輸出

**解決方法**:
- 確保已安裝 FFmpeg: `ffmpeg -version`
- 檢查音訊檔案格式是否支援
- 嘗試手動執行 vibe 命令查看錯誤訊息
- 檢查磁碟空間是否足夠

### 問題 4: 轉錄速度太慢

**解決方法**:
- 使用較小的模型（如 `tiny` 或 `base`）
- 考慮使用 whisper-ctranslate2（比官方版本快 4 倍）
- 如果有 GPU，確保 CUDA 已正確設定

## 進階設定

### 自訂模型選擇

編輯 vibe 腳本，修改預設模型：

```bash
# 可用模型: tiny, base, small, medium, large
# tiny: 最快但精度較低
# base: 速度和精度的平衡（推薦）
# small: 較好的精度，速度稍慢
# medium/large: 最佳精度，但非常慢
MODEL="base"
```

### 多語言支援

修改語言設定：

```bash
# 常用語言代碼
# zh: 中文
# en: 英文
# ja: 日文
# ko: 韓文
LANGUAGE="zh"
```

## 效能比較

| 工具 | 速度 | 精度 | GPU 支援 |
|------|------|------|---------|
| whisper-ctranslate2 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ✅ |
| openai-whisper | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ✅ |
| faster-whisper | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ✅ |

## 參考資源

- [OpenAI Whisper GitHub](https://github.com/openai/whisper)
- [whisper-ctranslate2 GitHub](https://github.com/Softcatala/whisper-ctranslate2)
- [FFmpeg 官方網站](https://ffmpeg.org/)
- [SRT 字幕格式說明](https://en.wikipedia.org/wiki/SubRip)
