#ifndef STATIONSTORAGE_H
#define STATIONSTORAGE_H

#include <QWidget>
#include "StationStorage/ccxnameedit.h"
#include "TableView/tableview.h"
namespace Ui {
class StationStorage;
}

class StationStorage : public QWidget
{
    Q_OBJECT

public:
    explicit StationStorage(QWidget *parent = nullptr);
    ~StationStorage();

    void Init();
    ccxnameedit *m_ccxnameedit;
    TableView* mCTableView;
    TableView* mCTableView2;
    TableView* mCTableView3;
    void InitTableView();
    void InitTableView2();
    void InitTableView3();
public slots:
    void pushButton1_Clicked();
private:
    Ui::StationStorage *ui;
};

#endif // STATIONSTORAGE_H
