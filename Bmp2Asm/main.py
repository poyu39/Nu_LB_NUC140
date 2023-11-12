import sys
from PIL import Image


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
    # 255 轉成 0，其他轉成 1
    print(bmp_bin)
    bmp_bin = [0 if x == 15 else 1 for x in bmp_bin]

    # 轉換成 16 進位
    # bmp_hex = []
    # for i in range(0, int(width * height / 8)):
    #     temp = ''
    #     for j in range(0, 7):
    #         temp += str(bmp_bin[i + j * width])
    #     temp = ''.join(reversed(temp))
    #     # bmp_hex.append(hex(int("".join([str(x) for x in bmp_bin[i:i+8]]), 2)))
    #     bmp_hex.append(hex(int(temp, 2)))

    # # 標準化格式
    # for i in range(len(bmp_hex)):
    #     if bmp_hex[i] == '0x0':
    #         bmp_hex[i] = '0x00'
    #     # 轉大寫
    #     bmp_hex[i] = str(bmp_hex[i]).upper()
    #     bmp_hex[i] = str(bmp_hex[i]).replace('X', 'x')
    #     # 補 0
    #     if len(bmp_hex[i]) == 3:
    #         bmp_hex[i] = bmp_hex[i][0:2] + '0' + bmp_hex[i][2]

    # print(bmp_hex)
    # 輸出成檔案
    with open(filename.split('.')[0] + '.c', 'w') as f:
        f.write('const unsigned char ' + filename.split('.')[0] + '[] = {')
        for i in range(len(bmp_bin)):
            if i % width == 0:
                f.write("\n\t")
            f.write(f'{bmp_bin[i]}, ')
        f.write('\n};')