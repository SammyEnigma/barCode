/*
 *@author: 缪庆瑞
 *@date:   2016.09.28
 *@update: 2026.09.01
 *@brief:  处理各类型的条形码编号，生成原始编码串或者条形码图片
 */
#include "barcode.h"
#include <QMap>
#include <QPainter>
#include <QDebug>

//EAN13条形码下方数字区高度(即防护条与数据条的差值)
#define EAN13_DIGIT_AREA_HEIGHT  16

/*
 *@brief:   生成Code128B类型的原始编码串
 *该接口生成原始的bs编码串，适用于根据编码串自行绘制条形码的情况，如果不想自绘，推荐使用
 *generateCode128BImage函数接口，直接生成绘制好的QImage图片。
 *@date:    2026.09.01
 *@param:   str:要编码的字符串
 *@return:  QByteArray:编码后的原始编码串(空表示参数出错)，'b'=单位黑线 's'=单位空白
 */
QByteArray BarCode::generateCode128BCode(const QString &str)
{
    if(str.isEmpty())
    {
        return QByteArray();
    }

    //code128每个id对应的bs编码串
    static QMap<int,QByteArray> code128IDMap = {
        {0,"bbsbbssbbss"},{1,"bbssbbsbbss"},{2,"bbssbbssbbs"},{3,"bssbssbbsss"},{4,"bssbsssbbss"},
        {5,"bsssbssbbss"},{6,"bssbbssbsss"},{7,"bssbbsssbss"},{8,"bsssbbssbss"},{9,"bbssbssbsss"},
        {10,"bbssbsssbss"},{11,"bbsssbssbss"},{12,"bsbbssbbbss"},{13,"bssbbsbbbss"},{14,"bssbbssbbbs"},
        {15,"bsbbbssbbss"},{16,"bssbbbsbbss"},{17,"bssbbbssbbs"},{18,"bbssbbbssbs"},{19,"bbssbsbbbss"},
        {20,"bbssbssbbbs"},{21,"bbsbbbssbss"},{22,"bbssbbbsbss"},{23,"bbbsbbsbbbs"},{24,"bbbsbssbbss"},
        {25,"bbbssbsbbss"},{26,"bbbssbssbbs"},{27,"bbbsbbssbss"},{28,"bbbssbbsbss"},{29,"bbbssbbssbs"},
        {30,"bbsbbsbbsss"},{31,"bbsbbsssbbs"},{32,"bbsssbbsbbs"},{33,"bsbsssbbsss"},{34,"bsssbsbbsss"},
        {35,"bsssbsssbbs"},{36,"bsbbsssbsss"},{37,"bsssbbsbsss"},{38,"bsssbbsssbs"},{39,"bbsbsssbsss"},
        {40,"bbsssbsbsss"},{41,"bbsssbsssbs"},{42,"bsbbsbbbsss"},{43,"bsbbsssbbbs"},{44,"bsssbbsbbbs"},
        {45,"bsbbbsbbsss"},{46,"bsbbbsssbbs"},{47,"bsssbbbsbbs"},{48,"bbbsbbbsbbs"},{49,"bbsbsssbbbs"},
        {50,"bbsssbsbbbs"}, {51,"bbsbbbsbsss"}, {52,"bbsbbbsssbs"}, {53,"bbsbbbsbbbs"}, {54,"bbbsbsbbsss"},
        {55,"bbbsbsssbbs"}, {56,"bbbsssbsbbs"}, {57,"bbbsbbsbsss"}, {58,"bbbsbbsssbs"}, {59,"bbbsssbbsbs"},
        {60,"bbbsbbbbsbs"}, {61,"bbssbssssbs"}, {62,"bbbbsssbsbs"}, {63,"bsbssbbssss"}, {64,"bsbssssbbss"},
        {65,"bssbsbbssss"}, {66,"bssbssssbbs"}, {67,"bssssbsbbss"}, {68,"bssssbssbbs"}, {69,"bsbbssbssss"},
        {70,"bsbbssssbss"}, {71,"bssbbsbssss"}, {72,"bssbbssssbs"}, {73,"bssssbbsbss"}, {74,"bssssbbssbs"},
        {75,"bbssssbssbs"}, {76,"bbssbsbssss"}, {77,"bbbbsbbbsbs"}, {78,"bbssssbsbss"}, {79,"bsssbbbbsbs"},
        {80,"bsbssbbbbss"}, {81,"bssbsbbbbss"}, {82,"bssbssbbbbs"}, {83,"bsbbbbssbss"}, {84,"bssbbbbsbss"},
        {85,"bssbbbbssbs"}, {86,"bbbbsbssbss"}, {87,"bbbbssbsbss"}, {88,"bbbbssbssbs"}, {89,"bbsbbsbbbbs"},
        {90,"bbsbbbbsbbs"}, {91,"bbbbsbbsbbs"}, {92,"bsbsbbbbsss"}, {93,"bsbsssbbbbs"}, {94,"bsssbsbbbbs"},
        {95,"bsbbbbsbsss"}, {96,"bsbbbbsssbs"}, {97,"bbbbsbsbsss"}, {98,"bbbbsbsssbs"}, {99,"bsbbbsbbbbs"},
        {100,"bsbbbbsbbbs"}, {101,"bbbsbsbbbbs"}, {102,"bbbbsbsbbbs"}, {103,"bbsbssssbss"}, {104,"bbsbssbssss"},
        {105,"bbsbssbbbss"}, {106,"bbsssbbbsbsbb"}
    };

    QByteArray bsCodeArray;
    bsCodeArray.append(code128IDMap.value(104));//开始位startB
    /* Code128B检验位,计算公式：
     * (开始位对应的ID值(B:104)＋每位数据在整个数据中的位置(从1开始)×每位数据对应的ID值)%103*/
    int checkId=104;
    int length = str.size();
    for(int i=0;i<length;i++)
    {
        ushort asciiVal = str.at(i).unicode();
        //Code128B可编码字符对应ascii码表的32-126可打印字符，ascii码值-32对应上面的ID
        if(asciiVal > 31 && asciiVal < 127)
        {
            int id = asciiVal-32;
            bsCodeArray.append(code128IDMap.value(id));//追加与字符对应的编码
            checkId += (i+1)*id;//计算校验值
        }
        else//含有非法(不可编码)字符
        {
            return QByteArray();
        }
    }
    checkId %=103;

    bsCodeArray.append(code128IDMap.value(checkId));//追加校验位的bs编码
    bsCodeArray.append(code128IDMap.value(106));//停止位 Stop
    return bsCodeArray;//返回编码好的bs串
}
/*
 *@brief:   生成Code128B类型的条形码图片
 *生成图片的高度：barCodeHeight+margins.top()+margins.bottom()
 *生成图片的宽度：编码后bs字符串的size()*penWidth+margins.left()+margins.right()
 *@date:    2026.09.01
 *@param:   str:要编码的字符串
 *@param:   barCodeHeight:条码高度  单位像素
 *@param:   penWidth:画笔宽度(单位黑线/空白的像素宽度) 单位像素 使用默认3即可,宽度太小可能扫描不出来
 *@param:   margins:四周的留白宽度 单位像素
 *@return:  QImage:条形码图片 空表示参数出错
 */
QImage BarCode::generateCode128BImage(const QString &str, const int &barCodeHeight,
                                      const int &penWidth, const QMargins &margins)
{
    QByteArray bsCodeArray = generateCode128BCode(str);
    if(bsCodeArray.isEmpty())
    {
        return QImage();
    }
    else
    {
        int image_width = bsCodeArray.size()*penWidth+margins.left()+margins.right();
        int image_height = barCodeHeight+margins.top()+margins.bottom();
        QImage image(image_width,image_height,QImage::Format_RGB32);
        /********在image上绘制条形码*******/
        QPainter painter(&image);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(Qt::white));
        painter.drawRect(0,0,image_width,image_height);//填充背景
        //画条形码
        painter.setPen(QPen(Qt::black,penWidth));//设置绘图画笔 黑色
        int x = margins.left();//条形码左顶点x位置
        int y = margins.top();//条形码左顶点y位置
        int end_y = y+barCodeHeight;//条码线条y底端位置

        int num = bsCodeArray.size();
        //绘制条形码线条
        for(int i=0;i<num;i++)
        {
            int start_x = x+i*penWidth;//条码线条x位置
            if(bsCodeArray.at(i)=='b')//画指定像素宽的黑线
            {
                painter.drawLine(start_x,y,start_x,end_y);
            }
        }

        return image;
    }
}
/*
 *@brief:   生成EAN13类型的原始编码串
 *该接口生成原始的01编码串，适用于根据编码串自行绘制条形码的情况，如果不想自绘，推荐使用
 *generateEAN13Image函数接口，直接生成绘制好的QImage图片。
 *@date:    2026.09.02
 *@param:   str:要编码的字符串,要求必须为12或13个数字
 *内部会自动计算一位校验码，如果输入12个数字则将其追加在尾部，如果输入13个数字，则判断最后一位和校验码是否一致，不一致则返回。
 *@return:  QByteArray:编码后的原始编码串(空表示参数出错)，'1'=单位黑线 '0'=单位空白
 */
QByteArray BarCode::generateEAN13Code(QString &str)
{
    if(str.size() != 12 && str.size() != 13)//长度错误
    {
        return QByteArray();
    }
    bool ok = false;
    str.toLongLong(&ok);
    if(!ok)//非纯数字错误
    {
        return QByteArray();
    }

    //前置码对应左侧数据符的编码方式AB 前置码为国家代码（0-9，最前面的数字）
    static QVector<QByteArray> preCodeWayList = {"AAAAAA","AABABB","AABBAB","AABBBA","ABAABB",
                                          "ABBAAB","ABBBAA","ABABAB","ABABBA","ABBABA"};
    //左侧数据符(6个) A编码方式下0-9对应编码值 1：单位黑线  0:单位空白
    static QVector<QByteArray> codeAValueList = {"0001101","0011001","0010011","0111101","0100011",
                                                "0110001","0101111","0111011","0110111","0001011"};
    //左侧数据符(6个) B编码方式下0-9对应编码值 1：单位黑线  0:单位空白
    static QVector<QByteArray> codeBValueList = {"0100111","0110011","0011011","0100001","0011101",
                                                "0111001","0000101","0010001","0001001","0010111"};
    //右侧数据符(5个) 0-9对应编码值 1：单位黑线  0:单位空白
    static QVector<QByteArray> codeCValueList = {"1110010","1100110","1101100","1000010","1011100",
                                                "1001110","1010000","1000100","1001000","1110100"};

    //C1 C2 CC checkId变量都是为了计算校验位
    int C1=0;//编号奇数位相加（13位数字是从1开始的，且不加第十三位）
    int C2=0;//编号偶数位相加和乘3
    int CC=0;//C1+C2取个位
    int checkId=0;//校验位 10-CC取个位

    QByteArray binCodeArray;//用来存放01串
    //左侧起始区域
    binCodeArray.append("00000000000");//11个模块的左侧空白区
    binCodeArray.append("101");//起始符
    //前置码
    int preNo = str.midRef(0,1).toInt();//转换得到第一个数字，即前置码
    C1+=preNo;
    QByteArray preCodeWay = preCodeWayList.at(preNo);
    //左侧数据符 6位
    for(int i=1;i<7;i++)
    {
        int left_number = str.midRef(i,1).toInt();
        if(i%2)
        {
            C2+=left_number;
        }
        else
        {
            C1+=left_number;
        }
        //将对应编码方式下的字符编码追加到字符串中
        if(preCodeWay.at(i-1)=='A')
        {
            binCodeArray.append(codeAValueList.at(left_number));
        }
        else if(preCodeWay.at(i-1)=='B')
        {
            binCodeArray.append(codeBValueList.at(left_number));
        }
    }
    //中间分隔符
    binCodeArray.append("01010");
    //右侧数据 5位
    for(int j=7;j<12;j++)
    {
        int right_number = str.midRef(j,1).toInt();
        if(j%2)
        {
            C2+=right_number;
        }
        else
        {
            C1+=right_number;
        }
        binCodeArray.append(codeCValueList.at(right_number));
    }
    //校验码
    CC=(C1+C2*3)%10;
    checkId=(10-CC)%10;
    //qDebug()<<"C2:"<<C2<<"C1:"<<C1<<"checkId:"<<checkId;
    if(str.size() == 13)
    {
        //输入的校验码错误
        if(str.midRef(12,1).toInt() != checkId)
        {
            return QByteArray();
        }
    }
    else
    {
        str.append(QString::number(checkId));
    }
    binCodeArray.append(codeCValueList.at(checkId));
    //右侧终止区域
    binCodeArray.append("101");//终止符
    binCodeArray.append("0000000");//7个模块的右侧空白区

    return binCodeArray;//返回编码好的01串
}
/*
 *@brief:   生成EAN13类型的条形码图片
 *生成图片的高度：barCodeMaxHeight+margins.top()+margins.bottom()
 *生成图片的宽度：编码后bs字符串的size()*penWidth+margins.left()+margins.right()
 *@date:    2026.09.02
 *@param:   str:要编码的字符串,要求必须为12或13个数字
 *内部会自动计算一位校验码，如果输入12个数字则将其追加在尾部，如果输入13个数字，则判断最后一位和校验码是否一致，不一致则返回。
 *@param:   showDigit:是否展示底部的数字
 *@param:   barCodeHeight:最长条码高度  单位像素,对应最长的线条高度(起始符、中间分隔符、终止符)
 *@param:   penWidth:画笔宽度(单位黑线/空白的像素宽度) 单位像素 使用默认3即可,宽度太小可能扫描不出来
 *@param:   margins:四周的留白宽度 单位像素
 *@return:  QImage:条形码图片 空表示参数出错
 */
QImage BarCode::generateEAN13Image(QString &str, const bool &showDigit,
                                   const int &barCodeMaxHeight, const int &penWidth,
                                   const QMargins &margins)
{
    QByteArray binCodeArray = generateEAN13Code(str);
    if(binCodeArray.isEmpty())
    {
        return QImage();
    }
    else
    {
        int image_width = binCodeArray.size()*penWidth+margins.left()+margins.right();
        int image_height = barCodeMaxHeight+margins.top()+margins.bottom();
        QImage image(image_width,image_height,QImage::Format_RGB32);
        /********在image上绘制条形码*******/
        QPainter painter(&image);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(Qt::white));
        painter.drawRect(0,0,image_width,image_height);//填充背景
        //画条形码
        painter.setPen(QPen(Qt::black,penWidth));//设置绘图画笔 黑色
        int x = margins.left();//条形码左顶点x位置
        int y = margins.top();//条形码左顶点y位置
        int end_y = y+barCodeMaxHeight-EAN13_DIGIT_AREA_HEIGHT;//条码码数据线条y底端位置

        int num = binCodeArray.size();
        //绘制条形码线条
        for(int i=0;i<num;i++)
        {
            int start_x = x+i*penWidth;//条码线条x位置
            if(binCodeArray.at(i) == '1')
            {
                //EAN13条形模块数是固定的，起始符 中间分隔符 终止符位置也是固定的，长度比数据线条多出一部分
                if((i>10&&i<14)||(i>55&&i<61)||(i>102&&i<106))
                {
                    painter.drawLine(start_x,y,start_x,end_y+EAN13_DIGIT_AREA_HEIGHT);
                }
                else
                {
                    painter.drawLine(start_x,y,start_x,end_y);
                }
            }
        }
        //绘制条码数字
        if(showDigit)
        {
            painter.setRenderHint(QPainter::TextAntialiasing);
            QFont font;//设置字体
            font.setPixelSize(16);
            painter.setFont(font);
            QRect rect(x+4*penWidth,end_y+2,7*penWidth,EAN13_DIGIT_AREA_HEIGHT);//前置符
            painter.drawText(rect,Qt::AlignCenter,str.mid(0,1));
            for(int i=0;i<6;i++)//左侧6个数据
            {
                QRect rect(x+(14+i*7)*penWidth,end_y+2,7*penWidth,EAN13_DIGIT_AREA_HEIGHT);
                painter.drawText(rect,Qt::AlignCenter,str.mid(i+1,1));
            }
            for(int i=0;i<6;i++)//右侧6个数据
            {
                QRect rect(x+(61+i*7)*penWidth,end_y+2,7*penWidth,EAN13_DIGIT_AREA_HEIGHT);
                painter.drawText(rect,Qt::AlignCenter,str.mid(i+7,1));
            }
        }

        return image;
    }
}
