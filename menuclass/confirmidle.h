#ifndef CONFIRMIDLE_H
#define CONFIRMIDLE_H

#include <QWidget>
#include "GlobalHeaders/Global.h"
namespace Ui {
class ConfirmIdle;
}

class ConfirmIdle : public QWidget
{
    Q_OBJECT

public:
    explicit ConfirmIdle(QWidget *parent = nullptr);
    ~ConfirmIdle();
  void Init();
  void InitShow(int ntpye,QString str,bool ncheckCQ=false,bool ncheckDW=false,bool ncheckFW=false);
private:
    Ui::ConfirmIdle *ui;
};

#endif // CONFIRMIDLE_H
