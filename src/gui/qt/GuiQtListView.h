
#ifndef GUIQTLISTVIEW_H
#define GUIQTLISTVIEW_H

#include <QListView>
#include <QAbstractListModel>

class GuiQtNavElement;

/** class MyQtListModel
    derived class of QAbstractListModel
 */
class MyQtListModel : public QAbstractListModel
{

public:
    MyQtListModel(QObject *parent = 0);
    virtual ~MyQtListModel();
    /** get data
	@param index model index
	@param role requested data role
    */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    /** get dataNode
	@param index model index
    */
    const GuiQtNavElement* dataNode(const QModelIndex &index) const;
    /** set tooltip data
	@param rowindex row index
	@param tooltip new tooltion string
    */
    void setToolTipData(long rowindex, const std::string& tooltip);
    /** get data flags
	@param index model index
    */
    Qt::ItemFlags flags(const QModelIndex &index) const;
    /** set tooltip data
	@param index model index
    */
    bool removeRows(int row, int count, const QModelIndex &parent);

    /** drop mine data (actual unused) */
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    /** get supported (drag) mine types */
    QStringList mimeTypes() const;
    /** get row count
     */
    int rowCount(const QModelIndex &parent= QModelIndex()) const;
    /** get supported drop mine types */
    Qt::DropActions supportedDropActions() const;

    /** add new item */
    long addItem(const QPixmap &pixmap, const GuiQtNavElement* navElem);

 private:
    QList<QPixmap> pixmaps;
    QList<QString> m_varnames;
    QList<const GuiQtNavElement*> m_navElements;
    QList<QString> m_tooltips;
};


/** class GuiQtListView
    derived class of QListView
 */
class GuiQtListView : public QListView
{

public:
    GuiQtListView(GuiQtNavigator* navigator);
    virtual ~GuiQtListView();
    MyQtListModel* model() { return m_model; }

    void removeAll();
    virtual void contextMenuEvent ( QContextMenuEvent * e );

 private:
    MyQtListModel* m_model;
    GuiQtNavigator* m_navigator;
};

#endif
