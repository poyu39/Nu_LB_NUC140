import sys
import time

from PIL import Image

'''
0 = 0xC0,0x20,0x10,0x10,0x20,0xC0,0x00,0x00,0x3F,0x40,0x80,0x80,0x40,0x3F,0x00,0x00

1       0 0 0 0 0 0 0 0
2       0 0 0 0 0 0 0 0
4       0 0 0 0 0 0 0 0
8       0 0 0 0 0 0 0 0
1       0 0 1 1 0 0 0 0
2       0 1 0 0 1 0 0 0
4       1 0 0 0 0 1 0 0
8       1 0 0 0 0 1 0 0
-----------------------
1       1 0 0 0 0 1 0 0
2       1 0 0 0 0 1 0 0
4       1 0 0 0 0 1 0 0
8       1 0 0 0 0 1 0 0
1       1 0 0 0 0 1 0 0
2       1 0 0 0 0 1 0 0
4       0 1 0 0 1 0 0 0
8       0 0 1 1 0 0 0 0
'''

if __name__ == "__main__":
    # 輸入檔案名稱, 寬, 高
    if len(sys.argv) == 4:
        filename = sys.argv[1]
        width = int(sys.argv[2])
        height = int(sys.argv[3])
    else:
        print("請輸入檔案名稱和長寬")
        sys.exit()
    bmp_img = Image.open(filename)
    bmp_bin = list(bmp_img.getdata())
    # 15 轉成 0，其他轉成 1
    bmp_bin = [0 if x == 15 else 1 for x in bmp_bin]

    # 壓縮成 16 進制
    bmp_hex = []
    for i in range(0, width * int(height / 8)):
        temp_bin = ''
        temp_hex = 0
        for j in range(0, 8):
            temp_bin += str(bmp_bin[i + j  * width])
        # 順序反轉
        temp_bin = temp_bin[::-1]
        temp_hex = f'{hex(int(str(temp_bin), 2))}'
        bmp_hex.append(temp_hex)

    # 標準化格式
    for i in range(len(bmp_hex)):
        if bmp_hex[i] == '0x0':
            bmp_hex[i] = '0x00'
        # 轉大寫
        bmp_hex[i] = str(bmp_hex[i]).upper()
        bmp_hex[i] = str(bmp_hex[i]).replace('X', 'x')
        # 補 0
        if len(bmp_hex[i]) == 3:
            bmp_hex[i] = bmp_hex[i][0:2] + '0' + bmp_hex[i][2]
        print(bmp_hex[i])
        # time.sleep(1)

    # 輸出成檔案
    with open(filename.split('.')[0] + '.c', 'w') as f:
        f.write('const unsigned char ' + filename.split('.')[0] + '_bin[] = {')
        for i in range(len(bmp_bin)):
            if i % width == 0:
                f.write("\n\t")
            f.write(f'{bmp_bin[i]}, ')
        f.write('\n};')
        f.write('\n\n')
        
        f.write('const unsigned char ' + filename.split('.')[0] + '_hex[] = {')
        for i in range(len(bmp_hex)):
            if i % width == 0:
                f.write("\n\t")
            f.write(f'{bmp_hex[i]}, ')
        f.write('\n};')