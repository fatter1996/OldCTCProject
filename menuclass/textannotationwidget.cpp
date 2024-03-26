#include "textannotationwidget.h"
#include "ui_textannotationwidget.h"
#include <qdebug.h>
TextAnnotationWidget::TextAnnotationWidget(QWidget *parent, TextAnnotation* pTextAnnotation) :
    QWidget(parent),
    ui(new Ui::TextAnnotationWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    this->installEventFilter(parent);
    this->setMouseTracking(true);
    ui->textEdit->installEventFilter(parent);
    ui->textEdit->setMouseTracking(true);
    blackcolor=QColor(255, 255, 255);
    textcolor=Qt::black;


    connect(ui->textEdit, &QTextEdit::textChanged, this, [=](){
        //限长100
        if(ui->textEdit->toPlainText().size()>100)
        {
            strAnnotation=ui->textEdit->toPlainText().left(100);
            ui->textEdit->setText(strAnnotation);
        }
        //ui->textEdit->setText(strAnnotation);
        //自适应宽度
        QFont font = ui->textEdit->font();
        QFontMetrics  fontMetrics(font);
        int w = fontMetrics.horizontalAdvance(ui->textEdit->toPlainText()) + 8;
        if(w > 80)
        {
            m_Rect.setWidth(w);
            m_Rect.setHeight(24);
            this->setGeometry(this->geometry().x(),this->geometry().y(), m_Rect.width(), m_Rect.height());
        }
        ui->textEdit->moveCursor(QTextCursor::End);
        //确认输入内容
        if(ui->textEdit->toPlainText().contains("\n"))
        {
            if(ui->textEdit->toPlainText() == "\n")
            {
                this->close();
                return;
            }

            textcolor = Qt::red;
            QString str = QString::number(textcolor.red())+","+QString::number(textcolor.green())+","+QString::number(textcolor.blue());
            QString str2 = QString::number(blackcolor.red())+","+QString::number(blackcolor.green())+","+QString::number(blackcolor.blue());
            ui->textEdit->setStyleSheet(QString("QTextEdit{color:rgb(%1);background-color:rgb(%2);}").arg(str).arg(str2));
            SetTextEditEN(false);

            strAnnotation=ui->textEdit->toPlainText().left(ui->textEdit->toPlainText().size()-1);
            ui->textEdit->setText(strAnnotation);
            if(pTextAnnotation)
            {
                pTextAnnotation->Init(m_Rect, strAnnotation, pTextAnnotation->m_nDiploid);
            }
            emit SaveSignal(pTextAnnotation);
        }
    });
}

TextAnnotationWidget::~TextAnnotationWidget()
{
    delete ui;
}
void TextAnnotationWidget::SetText(QString str)
{
    strAnnotation=str;
    ui->textEdit->setText(strAnnotation);
}
void TextAnnotationWidget::SetRect(QRectF rect)
{
    m_Rect=rect;
}
bool TextAnnotationWidget::GetMousePoint(QPoint p)
{
    return m_Rect.contains(p);
}
void TextAnnotationWidget::SetTextEditEN(bool b)
{
    ui->textEdit->setEnabled(b);
    if(b)
    {
        textcolor=Qt::black;
        QString str = QString::number(textcolor.red())+","+QString::number(textcolor.green())+","+QString::number(textcolor.blue());
        QString str2 = QString::number(blackcolor.red())+","+QString::number(blackcolor.green())+","+QString::number(blackcolor.blue());
        ui->textEdit->setStyleSheet(QString("QTextEdit{color:rgb(%1);background-color:rgb(%2);}").arg(str).arg(str2));
    }
}
void TextAnnotationWidget::SetTextFont()
{
    bool bFlag = true;
    QFont font = QFontDialog::getFont(&bFlag);
//    qDebug("当前选择的字体是[%s]-是否加粗[%d]-是否倾斜[%d]-字号[%d]", font.family().toUtf8().data(), font.bold(), font.italic(), font.pointSize());
    ui->textEdit->setFont(font);
}
void TextAnnotationWidget::SetTextcolor(int ntype)
{
    bool bFlag = true;
    QColorDialog mQColorDialog;
      ui->textEdit->setEnabled(true);
    mQColorDialog.show();
    if(mQColorDialog.exec()==QColorDialog::Accepted)
    {
        QColor color = mQColorDialog.selectedColor();

        QString str = QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue());
        if(ntype==0)
        {
            QString str2 = QString::number(blackcolor.red())+","+QString::number(blackcolor.green())+","+QString::number(blackcolor.blue());
            ui->textEdit->setStyleSheet(QString("QTextEdit{color:rgb(%1);background-color:rgb(%2);}").arg(str).arg(str2));
            textcolor=color;
        }
        else if(ntype==1)
        {
            QString str2 = QString::number(textcolor.red())+","+QString::number(textcolor.green())+","+QString::number(textcolor.blue());
            ui->textEdit->setStyleSheet(QString("QTextEdit{background-color:rgb(%1);color:rgb(%2);}").arg(str).arg(str2));
            blackcolor=color;
        }
    }
    ui->textEdit->setEnabled(false);
    update();
}

void TextAnnotationWidget::setEditFocus()
{
    ui->textEdit->setFocus();
}

void TextAnnotationWidget::EditEnter()
{
    ui->textEdit->append("\n");
}
