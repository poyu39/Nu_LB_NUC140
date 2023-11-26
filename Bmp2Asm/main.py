import sys
from PIL import Image

if __name__ == '__main__':
    if len(sys.argv) == 5:
        filename = sys.argv[1]
        width = int(sys.argv[2])
        height = int(sys.argv[3])
        hexorbin = str(sys.argv[4])
    else:
        print('請輸入檔案名稱, 長, 寬, 輸出格式(hex/bin)，(若長寬非圖片長寬，會自動縮放)')
        sys.exit()

    bmp_img = Image.open(filename)
    bmp_img = bmp_img.resize((width, height))
    bmp_img = bmp_img.convert('L')
    bmp_img = bmp_img.point(lambda x: 1 if x < 128 else 0, '1')
    bmp_bin = list(bmp_img.getdata())

    # 壓縮成 16 進制
    # 把 8 個 0 1 轉成 1 個 16 進制
    bmp_hex = []
    for k in range(0, int(height / 8)):
        for i in range(k * width * 8, k * width * 8 + width):
            temp_bin = ''
            temp_hex = 0
            for j in range(0, 8):
                pixel_index = i + j * width
                temp_bin += str(bmp_bin[pixel_index])
            # 順序反轉
            temp_bin = temp_bin[::-1]
            temp_hex = hex(int(temp_bin, 2))
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

    # 輸出成檔案
    filename = filename.split('\\')[-1].replace('.bmp', '_bmp')
    with open(filename + '.c', 'w') as f:
        if hexorbin == 'hex':
            f.write('uint8_t ' + filename + '_hex[] = {')
            for i in range(len(bmp_hex)):
                if i % width == 0:
                    f.write("\n\t")
                f.write(f'{bmp_hex[i]},')
            f.write('\n};')
        if hexorbin == 'bin':
            f.write('uint8_t ' + filename + '_bin[] = {')
            for i in range(len(bmp_bin)):
                if i % width == 0:
                    f.write("\n\t")
                f.write(f'{bmp_bin[i]},')
            f.write('\n};')