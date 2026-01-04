#!/usr/bin/env python3
"""
Whisper 語音轉錄腳本
此腳本使用 OpenAI Whisper 模型將音頻文件轉錄為文字
並即時輸出轉錄結果以供 Qt 應用程式讀取
"""

# 引入 Python 系統相關模組
import sys
# 引入 Python 作業系統相關模組
import os

def transcribe_audio(audio_file_path):
    """
    轉錄音頻文件
    
    Args:
        audio_file_path: 音頻文件的完整路徑
    """
    # 檢查音頻文件是否存在於指定路徑
    if not os.path.exists(audio_file_path):
        # 如果檔案不存在，輸出錯誤訊息到標準錯誤輸出
        print(f"錯誤: 找不到音頻文件: {audio_file_path}", file=sys.stderr)
        # 結束函式執行
        return
    
    # 輸出正在載入音頻文件的訊息
    print(f"正在載入音頻文件: {audio_file_path}")
    # 強制清空輸出緩衝區，立即顯示訊息
    sys.stdout.flush()
    
    try:
        # 嘗試導入 Whisper 模組
        import whisper
        
        # 輸出正在載入 Whisper 模型的訊息
        print("正在載入 Whisper 模型...")
        # 強制清空輸出緩衝區
        sys.stdout.flush()
        
        # 載入 Whisper 模型（使用 base 模型以獲得速度和精度的平衡）
        # 可選模型: tiny, base, small, medium, large
        model = whisper.load_model("base")
        
        # 輸出開始轉錄的訊息
        print("開始轉錄...")
        # 強制清空輸出緩衝區
        sys.stdout.flush()
        
        # 轉錄音頻文件，指定語言為中文
        result = model.transcribe(audio_file_path, language="zh")
        
        # 輸出完整轉錄標題
        print("\n=== 完整轉錄 ===")
        # 輸出轉錄的完整文字內容
        print(result["text"])
        # 強制清空輸出緩衝區
        sys.stdout.flush()
        
        # 輸出分段轉錄標題
        print("\n=== 分段轉錄 ===")
        # 遍歷所有轉錄的分段
        for segment in result["segments"]:
            # 格式化時間戳，顯示開始和結束時間（保留兩位小數）
            timestamp = f"[{segment['start']:.2f}s - {segment['end']:.2f}s]"
            # 取得分段的文字內容並移除前後空白
            text = segment['text'].strip()
            # 輸出時間戳和文字內容
            print(f"{timestamp} {text}")
            # 強制清空輸出緩衝區
            sys.stdout.flush()
        
        # 輸出轉錄完成的訊息
        print("\n轉錄完成!")
        # 強制清空輸出緩衝區
        sys.stdout.flush()
        
    except ImportError:
        # 如果無法導入 Whisper 模組，輸出錯誤訊息
        print("錯誤: 未安裝 Whisper 模組", file=sys.stderr)
        # 輸出安裝指令提示
        print("請使用以下命令安裝: pip install openai-whisper", file=sys.stderr)
        # 輸出 ffmpeg 依賴提示標題
        print("\n注意: Whisper 需要 ffmpeg，請確保已安裝:", file=sys.stderr)
        # 輸出 Ubuntu/Debian 系統的安裝指令
        print("  - Ubuntu/Debian: sudo apt install ffmpeg", file=sys.stderr)
        # 輸出 macOS 系統的安裝指令
        print("  - macOS: brew install ffmpeg", file=sys.stderr)
        # 輸出 Windows 系統的下載提示
        print("  - Windows: 從 https://ffmpeg.org 下載", file=sys.stderr)
        # 強制清空輸出緩衝區
        sys.stdout.flush()
    except Exception as e:
        # 捕捉其他所有異常，輸出錯誤訊息
        print(f"轉錄時發生錯誤: {str(e)}", file=sys.stderr)
        # 強制清空輸出緩衝區
        sys.stdout.flush()

def main():
    """主函數，程式進入點"""
    # 檢查命令列參數數量是否少於 2（程式名稱 + 音頻文件路徑）
    if len(sys.argv) < 2:
        # 輸出使用方法說明
        print("使用方法: python3 whisper_transcribe.py <音頻文件路徑>")
        # 輸出支援的音頻格式清單
        print("\n支援的音頻格式: mp3, wav, flac, m4a, ogg, aac")
        # 以錯誤狀態碼 1 結束程式
        sys.exit(1)
    
    # 取得第一個命令列參數作為音頻文件路徑
    audio_file_path = sys.argv[1]
    # 呼叫轉錄函式處理音頻文件
    transcribe_audio(audio_file_path)

# 檢查是否直接執行此腳本（而非被導入）
if __name__ == "__main__":
    # 呼叫主函數
    main()
