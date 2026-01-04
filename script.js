// 歌詞資料陣列，包含時間戳（以秒為單位）和對應的歌詞文字
const lyricsData = [
    { time: 0, text: "歡迎來到音樂播放器" },  // 0 秒時顯示的歌詞
    { time: 3, text: "這是一首示範歌曲" },    // 3 秒時顯示的歌詞
    { time: 6, text: "字幕會隨著音樂播放" },  // 6 秒時顯示的歌詞
    { time: 9, text: "當音樂進行到這裡" },    // 9 秒時顯示的歌詞
    { time: 12, text: "字幕就會同步顯示" },   // 12 秒時顯示的歌詞
    { time: 15, text: "你可以點擊任何一句歌詞" },  // 15 秒時顯示的歌詞
    { time: 18, text: "音樂會自動跳轉到那個位置" },  // 18 秒時顯示的歌詞
    { time: 21, text: "這樣就能快速導航" },   // 21 秒時顯示的歌詞
    { time: 24, text: "找到你喜歡的部分" },   // 24 秒時顯示的歌詞
    { time: 27, text: "享受音樂的美好時光" }, // 27 秒時顯示的歌詞
    { time: 30, text: "每一句都有精確的時間點" },  // 30 秒時顯示的歌詞
    { time: 33, text: "讓歌詞完美同步" },     // 33 秒時顯示的歌詞
    { time: 36, text: "就像在 KTV 唱歌一樣" }, // 36 秒時顯示的歌詞
    { time: 39, text: "這就是即時字幕功能" }, // 39 秒時顯示的歌詞
    { time: 42, text: "感謝您的使用" }        // 42 秒時顯示的歌詞
];

// 取得 HTML 頁面中的音頻播放器元素
const audioPlayer = document.getElementById('audio-player');
// 取得 HTML 頁面中的歌詞容器元素
const lyricsContainer = document.getElementById('lyrics-container');

// 初始化歌詞顯示的函式
function initLyrics() {
    // 清空歌詞容器的內容
    lyricsContainer.innerHTML = '';
    
    // 遍歷歌詞資料陣列中的每一項
    lyricsData.forEach((lyric, index) => {
        // 創建一個新的 div 元素作為歌詞行
        const lyricLine = document.createElement('div');
        // 設定歌詞行的 CSS 類別名稱
        lyricLine.className = 'lyrics-line';
        // 設定歌詞行的文字內容
        lyricLine.textContent = lyric.text;
        // 在元素上儲存時間戳資料
        lyricLine.dataset.time = lyric.time;
        // 在元素上儲存索引資料
        lyricLine.dataset.index = index;
        
        // 為歌詞行添加點擊事件監聽器
        lyricLine.addEventListener('click', function() {
            // 將音頻播放器的當前時間設定為此歌詞的時間戳
            audioPlayer.currentTime = parseFloat(this.dataset.time);
            // 如果音頻處於暫停狀態
            if (audioPlayer.paused) {
                // 嘗試播放音頻，若瀏覽器阻止自動播放則捕捉錯誤
                audioPlayer.play().catch(e => {
                    // 輸出錯誤訊息到主控台
                    console.log('Auto-play prevented by browser:', e);
                });
            }
        });
        
        // 將歌詞行元素添加到歌詞容器中
        lyricsContainer.appendChild(lyricLine);
    });
}

// 根據當前播放時間更新啟用的歌詞行
function updateLyrics() {
    // 取得音頻播放器的當前播放時間
    const currentTime = audioPlayer.currentTime;
    // 取得所有歌詞行元素
    const lyricLines = document.querySelectorAll('.lyrics-line');
    
    // 初始化當前啟用的歌詞索引為 -1（表示沒有啟用的歌詞）
    let activeIndex = -1;
    
    // 從最後一句歌詞開始往前查找當前應該啟用的歌詞
    for (let i = lyricsData.length - 1; i >= 0; i--) {
        // 如果當前時間大於等於此歌詞的時間戳
        if (currentTime >= lyricsData[i].time) {
            // 設定此歌詞為當前啟用的歌詞
            activeIndex = i;
            // 跳出迴圈
            break;
        }
    }
    
    // 更新所有歌詞行的樣式
    lyricLines.forEach((line, index) => {
        // 移除歌詞行的 active 和 passed 類別
        line.classList.remove('active', 'passed');
        
        // 如果此歌詞是當前啟用的歌詞
        if (index === activeIndex) {
            // 添加 active 類別（高亮顯示）
            line.classList.add('active');
            // 自動捲動到此歌詞行，使其顯示在視窗中央
            line.scrollIntoView({
                behavior: 'smooth',  // 使用平滑捲動動畫
                block: 'center'      // 將元素置於視窗中央
            });
        } else if (index < activeIndex) {
            // 如果此歌詞在當前啟用的歌詞之前，添加 passed 類別
            line.classList.add('passed');
        }
    });
}

// 為音頻播放器添加時間更新事件監聽器，當播放時間改變時呼叫 updateLyrics 函式
audioPlayer.addEventListener('timeupdate', updateLyrics);

// 為音頻播放器添加元資料載入完成事件監聽器
audioPlayer.addEventListener('loadedmetadata', function() {
    // 輸出音頻時長到主控台
    console.log('Audio loaded, duration:', audioPlayer.duration);
});

// 為音頻播放器添加錯誤事件監聽器
audioPlayer.addEventListener('error', function(e) {
    // 輸出錯誤訊息到主控台
    console.error('Audio error:', e);
    // 輸出提示訊息，說明音頻檔案未找到但介面仍可使用
    console.log('Note: Audio file not found. The interface is ready but you need to add an audio file named "sample-audio.mp3"');
});

// 當頁面載入完成時執行初始化函式
document.addEventListener('DOMContentLoaded', function() {
    // 呼叫歌詞初始化函式
    initLyrics();
    // 輸出初始化完成訊息到主控台
    console.log('Lyrics player initialized');
    // 輸出歌詞總行數到主控台
    console.log('Total lyrics lines:', lyricsData.length);
});

// 為音頻播放器添加播放事件監聽器
audioPlayer.addEventListener('play', function() {
    // 輸出播放狀態到主控台
    console.log('Audio playing');
});

// 為音頻播放器添加暫停事件監聽器
audioPlayer.addEventListener('pause', function() {
    // 輸出暫停狀態到主控台
    console.log('Audio paused');
});

// 為音頻播放器添加播放結束事件監聽器
audioPlayer.addEventListener('ended', function() {
    // 取得所有歌詞行元素
    const lyricLines = document.querySelectorAll('.lyrics-line');
    // 遍歷所有歌詞行
    lyricLines.forEach(line => {
        // 移除所有歌詞行的 active 和 passed 類別，重置為初始狀態
        line.classList.remove('active', 'passed');
    });
});
