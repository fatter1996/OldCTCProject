#ifndef STAGECHRONICLE_H
#define STAGECHRONICLE_H

#include <QWidget>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeWidgetItem>
namespace Ui {
class StageChronicle;
}

class StageChronicle : public QWidget
{
    Q_OBJECT

public:
    explicit StageChronicle(QWidget *parent = nullptr);
    ~StageChronicle();
    void InitTree();
    QTreeWidgetItem *topItem_Station;

signals:
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private:
    Ui::StageChronicle *ui;
};

#endif // STAGECHRONICLE_H
