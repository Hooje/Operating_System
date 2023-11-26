# 簡介
這個存儲庫包含三個主要的 C 程式 - server.c、client.c 和 page_table_driver.c，分別處理共享記憶體和頁表功能。

server.c 與 client.c
這兩個檔案實現了使用 System V 共享記憶體 IPC 的功能。程式使用 ftok 生成與共享記憶體段關聯的金鑰，提供了更穩健的金鑰生成方法。

page_table_driver.c
這個程式與一個核心驅動程式進行互動，使用 ioctl 系統呼叫來執行頁表相關操作。它首先打開 /dev/os 檔案以獲得文件描述符 (fd)。接著，使用 ioctl 指定 fd 以取得 CR3 值，這是頁表管理的重要部分。CR3 值與虛擬記憶體相關，並在 trans() 函數中使用其他參數。trans() 函數使用位元運算從虛擬記憶體地址中提取偏移和其他相關參數。然後，使用 CR3 值對這些參數進行計算以確定相應的實體地址。

最後，程式再次使用 ioctl 來將獲得的實體地址映射到另一個本地地址。