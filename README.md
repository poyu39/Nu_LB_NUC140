# Nu-LB-NUC140

IECS213 微處理機系統 Nu-LB-NUC140 函式庫

## Tool

### Bmp2Asm

將 bmp 轉換為 c array 的小工具

usage: `main.py <檔案名稱> <長> <寬> <輸出格式(hex/bin)>`


## Library

### NewLCD

基於 buffer 動態更新解決 LCD 閃爍問題，並加上一些常用功能。

[NewLCD.h](Library/Nu-LB-NUC140/Include/NewLCD.h)

[NewLCD.c](Library/Nu-LB-NUC140/Source/NewLCD.c)


### NewScanKey

使用 PA0, 1, 2 interrupt 解決按鍵掃描問題。

[NewScanKey.h](Library/Nu-LB-NUC140/Include/NewScanKey.h)

[NewScanKey.c](Library/Nu-LB-NUC140/Source/NewScanKey.c)


### NewSevenSegment

使用 interrupt 背景控制七段顯示器，並直接對 PE 寫入資料。

[NewSevenSegment.h](Library/Nu-LB-NUC140/Include/NewSevenSegment.h)

[NewSevenSegment.c](Library/Nu-LB-NUC140/Source/NewSevenSegment.c)


## NewTrunButton

使用 ADC, PWM 讀取可變電阻。

[NewTrunButton.h](Library/Nu-LB-NUC140/Include/NewTrunButton.h)

[NewTrunButton.c](Library/Nu-LB-NUC140/Source/NewTrunButton.c)