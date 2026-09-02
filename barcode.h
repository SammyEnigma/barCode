/*
 *@author: 缪庆瑞
 *@date:   2016.09.28
 *@update: 2026.09.01
 *@brief:  处理各类型的条形码编号，生成原始编码串或者条形码图片
 */
#ifndef BARCODE_H
#define BARCODE_H

#include <QString>
#include <QImage>
#include <QMargins>

class BarCode
{
public:
    /* 生成Code128B类型的条形码(变长)
     * Code128包括A、B、C三种不同的编码类型，涵盖了ASCII码中128个字元的表示，不过最常用的是Code128B，
     * 它可以编码数字、大小写字母和特殊字符(即ascii码表中的可打印字符)。
     * Code128B的编码规则：开始位+数据位+校验位+停止位*/
    static QByteArray generateCode128BCode(const QString &str);
    static QImage generateCode128BImage(const QString &str,
                                        const int &barCodeHeight = 150,const int &penWidth = 3,
                                        const QMargins &margins = QMargins(16,16,16,16));


    /* 生成EAN13类型条形码(定长)
     * EAN13码包含13位数字(0-9)，第一位前置码，最后一位校验码(通过前12位运算自动生成)，缺一不可。
     * 数据字符和校验码都是占7个条形模块，整体组成：左侧空白区(11)+前置码+起始符(3)+左侧数据符(42(6*7))+
     * 中间分隔符(5)+右侧数据符(35(5*7))+校验符(7)+终止符(3)+右侧空白区(7)*/
    static QByteArray generateEAN13Code(QString &str);
    static QImage generateEAN13Image(QString &str,const bool &showDigit = true,
                                     const int &barCodeMaxHeight = 150,const int &penWidth = 3,
                                     const QMargins &margins = QMargins(0,20,4,20));
};

#endif // BARCODE_H
