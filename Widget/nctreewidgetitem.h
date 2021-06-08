#ifndef NCTREEWIDGETITEM_H
#define NCTREEWIDGETITEM_H

#include <QTreeWidgetItem>

class NCTreeWidgetItem : public QTreeWidgetItem
{
public:
    NCTreeWidgetItem(const QStringList &strings, QMenu *menu = nullptr);


    QMenu *Menu();

private:
    QMenu *m_menu;
};

#endif // NCTREEWIDGETITEM_H
