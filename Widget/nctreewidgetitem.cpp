#include "nctreewidgetitem.h"

NCTreeWidgetItem::NCTreeWidgetItem(const QStringList &strings, QMenu *menu)
    : QTreeWidgetItem(strings)
    , m_menu(menu)
{

}

QMenu *NCTreeWidgetItem::Menu()
{
    return m_menu;
}
