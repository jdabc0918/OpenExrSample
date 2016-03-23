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
};

void readHeader(HeaderInfo  &info)
{
    FILE *m_fp;

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

int main(int argc, char **argv)
{   
	return 0;
}

