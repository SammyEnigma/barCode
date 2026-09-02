#include "widget.h"
#include "barcode.h"
#include <QBoxLayout>
#include <QMessageBox>
#include <QTime>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    label = new QLabel("编号：",this);
    lineEdit = new QLineEdit(this);
    produceCode128Button = new QPushButton("生成code128",this);
    produceEAN13Button = new QPushButton("生成EAN13",this);
    savePictureButton = new QPushButton("保存到图片",this);
    barCodeBox = new QGroupBox("条形码区域：",this);
    barCodeLabel = new QLabel(this);

    QHBoxLayout *hBoxLayout = new QHBoxLayout();//第一行水平布局
    hBoxLayout->addWidget(label);
    hBoxLayout->addWidget(lineEdit);
    hBoxLayout->addWidget(produceCode128Button);
    hBoxLayout->addWidget(produceEAN13Button);
    hBoxLayout->addWidget(savePictureButton);

    QHBoxLayout *hBoxLayout2 = new QHBoxLayout(barCodeBox);//第二行条形码区域水平布局
    hBoxLayout2->addWidget(barCodeLabel);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);//整体垂直布局
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addWidget(barCodeBox);

    connect(produceCode128Button,SIGNAL(clicked()),this,SLOT(produceBarCode128Slot()));
    connect(produceEAN13Button,SIGNAL(clicked()),this,SLOT(produceBarCodeEAN13Slot()));
    connect(savePictureButton,SIGNAL(clicked()),this,SLOT(savePictureSlot()));

    this->resize(600,400);
}

Widget::~Widget()
{
}
//生成Code128B类型条码
void Widget::produceBarCode128Slot()
{
    QString code=lineEdit->text();//获取输入的编号
    QImage code128BImage = BarCode::generateCode128BImage(code);
    if(code128BImage.isNull())
    {
        QMessageBox::warning(this,"生成失败","请输入合法的编号！");
    }
    else
    {
        barCodeLabel->setFixedSize(code128BImage.size());
        barCodeLabel->setPixmap(QPixmap::fromImage(code128BImage));
    }
}

//生成EAN13条形码
void Widget::produceBarCodeEAN13Slot()
{
    QString code=lineEdit->text();//获取输入的编号
    QImage EAN13Image = BarCode::generateEAN13Image(code);
    if(EAN13Image.isNull())
    {
        QMessageBox::warning(this,"生成失败","请输入合法的编号！");
    }
    else
    {
        barCodeLabel->setFixedSize(EAN13Image.size());
        barCodeLabel->setPixmap(QPixmap::fromImage(EAN13Image));
    }
}
//保存条码图片
void Widget::savePictureSlot()
{
    const QPixmap *pixmap = barCodeLabel->pixmap();
    if(pixmap)
    {
        QString filename = QString("条形码%1.png").arg(QTime::currentTime().toString("HHmmss"));
        int width = pixmap->width();
        int height = pixmap->height();
        bool save_ret = false;
        qDebug()<<width<<height<<width/height;
        if(width/height > 3)//当宽高比例大于4倍时，则按照高度的四倍缩放宽度,避免太宽了
        {
            QPixmap save_pixmap = pixmap->scaled(height*4,height,Qt::IgnoreAspectRatio,
                                                 Qt::SmoothTransformation);//将图片伸缩到指定大小，默认参数忽略宽高比例
            save_ret = save_pixmap.save(filename,"PNG",100);
        }
        else
        {
            save_ret = pixmap->save(filename,"PNG",100);
        }

        if(save_ret)
        {
            QMessageBox::information(this,"保存成功","图片已保存");
        }
        else
        {
            QMessageBox::warning(this,"保存失败","图片保存失败");
        }
    }
    else
    {
        QMessageBox::warning(this,"保存失败","请先生成条形码！");
    }

}


