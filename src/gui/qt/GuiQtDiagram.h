
#ifndef GUIQTDIAGRAM_H
#define GUIQTDIAGRAM_H

#include <QList>
#include <QPoint>
#include <QPixmap>
#include <QWidget>
#include <QGraphicsView>

#include "gui/qt/GuiQtNavigator.h"
#include "gui/GuiMenuButtonListener.h"
#include "gui/qt/GuiQtDiagramItem.h"

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
QT_END_NAMESPACE

class GuiNavElement;
class GuiQtNavElement;
class GuiQtDiagramScene;
class GuiQtDiagramPixmapItem;
class XferDataItem;

class GuiQtDiagram : public QGraphicsView
{
    Q_OBJECT

public:
  GuiQtDiagram(GuiQtNavigator* navigator, GuiNavigator::RootsVector& roots,
               GuiQtDiagramScene* scene, QWidget *parent = 0);
    void clear();
    void copy(std::string filename=std::string());
    GuiQtDiagramPixmapItem* addPixmapItem( GuiQtNavElement* navElem );
    void refreshConnections();
    void refreshPixmapItem(GuiQtNavElement* navElement, GuiQtDiagramPixmapItem* item);
    static void getPixmap(QPixmap& icon, const std::string& name, bool bVarname=false);
    static void getCompositePixmap(QPixmap&  icon, const std::string& varname, double scale=1.0);
    void newConnection(GuiQtDiagramPixmapItem* startItem,
		       GuiQtDiagramPixmapItem* endItem);
    void removeConnection(GuiQtDiagramPixmapItem* startItem,
			  GuiQtDiagramPixmapItem* endItem);
    void removeElement(const std::vector<GuiQtDiagramPixmapItem*>& items);
    void setGridFactor(int grid_x, int grid_y) { GRID_LEN_X= grid_x; GRID_LEN_X = grid_y; }
    void setCompositeScaleFactor(double fac) { m_composite_scale_factor = fac; }
    GuiQtDiagramConnection::ConnectType getConnectionType() { return m_connectType; };
    void setConnectionType(GuiQtDiagramConnection::ConnectType ctype) { m_connectType = ctype; };
    QPoint getGridPoint(const QPointF &) const;
    QPointF toGridPoint(const QPointF &) const;
    static    bool isCompositePixmap(const std::string& varname);
    void initShortcuts();
    void setItemSelection(const std::string& id, bool bSelected=true);
    void saveItemSelection();
    void restoreItemSelection();
    void getConnections(std::vector<std::string>& vecA,
                        std::vector<std::string>& vecB,
                        std::vector<GuiNavigator::ConnectionAttr>& attr) const;
    void getItemSelection(std::vector<std::string>& select_items) const;
    double getDiagramConnectionRatio();

private slots:
    void slot_itemMoved(std::vector<GuiQtDiagramPixmapItem*>, const QPointF &);
    void slot_selectedChange(GuiQtDiagramPixmapItem *item);
    void slot_activated(GuiQtDiagramPixmapItem *item);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

    class ScaleListener : public GuiMenuButtonListener
      {
      public:
	ScaleListener( GuiQtDiagram *diagram, double fac );
	virtual ~ScaleListener() {}
	virtual void ButtonPressed();
	virtual JobAction* getAction() { return 0; }
	void    validateState();
      private:
	GuiQtDiagram  *m_diagram;
	double         m_fac;
	qreal          m_min;
	qreal          m_max;
      };

    class CopyListener : public GuiMenuButtonListener
      {
      public:
	CopyListener( GuiQtDiagram *diagram );
	virtual ~CopyListener() {}
	virtual void ButtonPressed();
	virtual JobAction* getAction() { return 0; }
      private:
	GuiQtDiagram  *m_diagram;
      };

  class ConnectionTypeListener : public GuiMenuButtonListener
  {
  public:
    ConnectionTypeListener( GuiQtDiagram *diagram );
    virtual ~ConnectionTypeListener() {}
    virtual void ButtonPressed();
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQtDiagram  *m_diagram;
  };

    class DeleteElementListener : public GuiMenuButtonListener
      {
      public:
	DeleteElementListener( GuiQtDiagram *diagram );
	virtual ~DeleteElementListener() {}
	virtual void ButtonPressed();
	virtual JobAction* getAction() { return 0; }
      private:
	GuiQtDiagram  *m_diagram;
      };

    class ItemFragment
      {
      public:
	ItemFragment(const std::string& id)
	: m_id(id) {}
	bool operator==(const ItemFragment& item) {
	  if (item.m_id == m_id)
	    return true;
	  return false;
	}
      private:
	std::string m_id;
      };

private:
    int findPiece(const QRect &pieceRect) const;
    const QRect targetSquare(const QPoint &position, const QPixmap& icon=QPixmap()) const;
    QPointF mapViewToSceneCenteredGridPoint(const QPoint& pos, const QPixmap& pixmap) const;
    static void getDiagramPixmap(QPixmap& icon, const std::string& name);

    void call_function(std::string varname, JobElement::CallReason reason);
    void call_function(const std::vector<std::string>& varnames, JobElement::CallReason reason);
    void call_function(std::string varname, QPoint pos, JobElement::CallReason reason);
    void call_function(const std::vector<std::string>& varnames, QPoint pos,
                       JobElement::CallReason reason);
    void call_function(std::string varname1, std::string varname2, JobElement::CallReason reason);
    void updateConnectionAttr(std::string& in_node0, std::string& in_node1,
                              GuiNavigator::ConnectionAttr& cattr) const;
    QRect highlightedRect;

    GuiQtNavigator*              m_navigator;
    GuiNavigator::RootsVector&   m_roots;

    int GRID_LEN_X;
    int GRID_LEN_Y;
    GuiQtDiagramConnection::ConnectType m_connectType;
    double m_composite_scale_factor;
    ScaleListener m_scacleInMenu;
    ScaleListener m_scacleOutMenu;
    CopyListener m_copyMenu;
    ConnectionTypeListener m_connectionTypeMenu;
    DeleteElementListener m_deleteElement;
    std::map<std::string, GuiMenuButtonListener*> m_addPopupMenu;

    std::vector<ItemFragment> m_selectedItems;
};

#endif
