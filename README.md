# 快取模擬器（Cache Simulator with NRU Replacement）

使用 C++ 撰寫的快取記憶體模擬器，模擬快取的存取行為與替換策略，  
使用 **NRU（Not Recently Used）替換策略** 模擬資料寫入與替換流程。

**題目來源**：此專案為 **國立清華大學 黃婷婷教授《計算機結構》課程期末報告**。

---

##  功能說明

- 支援自行設定的快取參數（位址位元數、區塊大小、Set 數量、相聯度）
- 根據設定自動計算 offset / index / tag 位元數與位置
- 模擬記憶體讀取行為，並計算每筆存取是 hit 或 miss
- 使用 **NRU 替換策略** 模擬快取替換行為
- 輸出模擬結果報告檔 `.rpt`，內容包括：
  - 快取設定摘要
  - 位址切割的 index 位元位置（由左至右標示）
  - 每筆存取結果（binary address + hit/miss）
  - 最後統計 miss 次數

---

## 資料結構使用說明

| 功能             | 使用資料結構                           |
|------------------|----------------------------------------|
| 快取內容         | `vector<vector<int>> cache`：二維陣列，每個 set 有多個 way |
| NRU 狀態記錄     | `vector<vector<bool>> nru`：與 cache 對應的 bool 二維陣列 |
| 位址記錄         | `vector<int> addresses`：儲存所有輸入的二進位地址 |
| 模擬結果記錄     | `vector<bool> hitOrMissList`：儲存每筆存取是否為 hit |


./project cache1.org reference1.lst index.rpt


