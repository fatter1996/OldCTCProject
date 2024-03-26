#ifndef TEXTANNOTATIONWIDGET_H
#define TEXTANNOTATIONWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QFontDialog>
#include <QColorDialog>
#include "MyStation/textannotation.h"

namespace Ui {
class TextAnnotationWidget;
}

class TextAnnotationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TextAnnotationWidget(QWidget *parent = nullptr, TextAnnotation* pTextAnnotation = nullptr);
    ~TextAnnotationWidget();

    void SetText(QString str);
    void SetRect(QRectF rect);
    bool GetMousePoint(QPoint p);
    void SetTextEditEN(bool b);
    void SetTextFont();
    void SetTextcolor(int ntype);

    void setEditFocus();

    void EditEnter();

public:
    QString strAnnotation;
    QRectF m_Rect;          //控件显示区域
    QColor textcolor;
    QColor blackcolor;
signals:
    void SaveSignal(TextAnnotation* pTextAnnotation = nullptr);
private:
    Ui::TextAnnotationWidget *ui;
};

#endif // TEXTANNOTATIONWIDGET_H
