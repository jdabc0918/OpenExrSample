// ExrReaderSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

typedef struct HeaderInfo
{
    //  バージョンなど
    int magic_number;
    int version_number;

    //  チャンネル情報
    int ch_data_size;
    std::vector<std::string> ch_name;
    std::vector<std::string> ch_type;

    //  画像サイズ
    int width, height;

    //  出力
    void print()
    {
        std::cout
            << "--- OpenEXRファイル情報" << "\n"
            << "    バージョン:" << version_number << "\n"
            << "    チャンネル:" << std::endl;
        for (int i = 0; i < (int)ch_name.size(); i++)
        {
            std::cout << "       ch[" << i << "] -> " << ch_name[i] << "(" << ch_type[i] << ")" << std::endl;
        }
        std::cout
            << "    画像サイズ:(" << width << "," << height << ")\n"
            << "---"            << std::endl;
    }
};

void readHeader(HeaderInfo  &info)
{
    FILE *m_fp = fopen("C:\\Users\\Yosuke\\Desktop\\untitled.exr", "rb");

    //  magic number
    fseek(m_fp, 0, SEEK_SET);
    fread(&info.magic_number, 4, 1, m_fp);
    //  version
    fread(&info.version_number, 1, 1, m_fp); //  version number
    fseek(m_fp, 3, SEEK_CUR);   //  flags(skip)
    //  channnel
    fseek(m_fp, 16, SEEK_CUR);   //  text(skip)
    fread(&info.ch_data_size, 4, 1, m_fp);    //  channnel data size
    for (int i = 0; i < (info.ch_data_size - 1) / 18; i++)  //  channel name & type
    {
        //  name
        char ch_name_c[2];   
        fread(ch_name_c, 1, 2, m_fp);
        std::string ch_name_str(ch_name_c);
        info.ch_name.push_back(ch_name_str);

        //  type
        int type_index;
        fread(&type_index, 4, 1, m_fp);
        switch (type_index)
        {
        case 0:
            info.ch_type.push_back("UINT");
            break;
        case 1:
            info.ch_type.push_back("HALF");
            break;
        case 2:
            info.ch_type.push_back("FLOAT");
            break;
        default:
            info.ch_type.push_back("null");
            break;
        }

        fseek(m_fp, 12, SEEK_CUR);  //  (skip)
    }
    fseek(m_fp, 1, SEEK_CUR);   //  (skip)
    //  compression
    fseek(m_fp, 29, SEEK_CUR);   //  (skip)
    //  data window
    fseek(m_fp, 29, SEEK_CUR);   //  (skip)
    fread(&info.width, 4, 1, m_fp); //  width
    fread(&info.height, 4, 1, m_fp); //  height
    info.width += 1;
    info.height += 1;
}

int SearchText(unsigned char *data, int dataNum, std::string text)
{
    //  見つからなかったら-1を返す
    if (dataNum < (int)text.length()) return -1;

    int loc = -1;
    for (int i = 0; i <= dataNum - (int)text.length(); i++)
    {
        std::string tmp;
        for (int j = 0; j < (int)text.length(); j++)
        {
            tmp += (char)data[i + j];
        }
        if (tmp == text)
        {
            loc = i;
            break;
        }
    }

    return loc;
}

void readHeader2(HeaderInfo  &info)
{
    FILE *m_fp = fopen("C:\\Users\\Yosuke\\Desktop\\untitled2.exr", "rb");

    //  一旦データをすべて読み込む
    int dataByte = 0;
    while (fgetc(m_fp) != EOF) dataByte++;   //  空読みしてバイト数を取得    
    unsigned char *data = new unsigned char[dataByte];
    fseek(m_fp, 0, SEEK_SET);
    fread(data, sizeof(unsigned char), dataByte, m_fp);

    //  ワーク変数
    int ret, tmp;

    //  magic number
    info.magic_number = ((int*)data)[0];
    //  version
    info.version_number = ((int*)data)[1];
    //  channnel
    ret = SearchText(data, dataByte, "channels") + 16;
    info.ch_data_size = *(int*)&data[ret];
    ret += 4;
    for (int i = 0; i < (info.ch_data_size - 1) / 18; i++)  //  channel name & type
    {
        //  index
        tmp = ret + i * 18;
        //  channel name
        std::string ch_name_str((const char *)&data[tmp]);
        info.ch_name.push_back(ch_name_str);
        //  type
        tmp += 2;
        int type_index = *(int*)&data[tmp];
        switch (type_index)
        {
        case 0:
            info.ch_type.push_back("UINT");
            break;
        case 1:
            info.ch_type.push_back("HALF");
            break;
        case 2:
            info.ch_type.push_back("FLOAT");
            break;
        default:
            info.ch_type.push_back("null");
            break;
        }
    }
    //  compression
    ret = SearchText(data, dataByte, "compression") + 24;
    if (*(int*)&data[ret] != 1)
    {
        std::cout << "無圧縮じゃないようです。。。" << std::endl;
        return;
    }
    //  data window
    ret = SearchText(data, dataByte, "dataWindow") + 29;
    info.width = *(int*)&data[ret] + 1;
    info.height = *(int*)&data[ret + 4] + 1;
    //  start of scan line offset table
    ret = SearchText(data, dataByte, "xDensity") + 24;  //ここいまいち。"xDensity"という文字列を見つけて+24
    //  ★Start of Pixel Data
    ret += 8 * info.height; //  オフセットテーブルを飛ばす

    info.print();
}

int main(int argc, char **argv)
{   
    HeaderInfo hi;
    readHeader2(hi);

    printf("hoge\n");

    hi.print();
	return 0;
}

