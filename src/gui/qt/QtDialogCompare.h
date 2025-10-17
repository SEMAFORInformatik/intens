
#if !defined(GUI_QT_COMPARE_DIALOG_H)
#define GUI_QT_COMPARE_DIALOG_H

#include <vector>
#include <jsoncpp/json/value.h>
#include "gui/DialogCompare.h"
#include "gui/GuiButtonListener.h"
#include "gui/qt/GuiQtDialog.h"
#include "gui/qt/GuiQtForm.h"
#include "app/UserAttr.h"

class GuiQtForm;
class GuiOrientationContainer;
class XferDataItem;
class GuiFieldgroup;

/** Mit einem QtDialogCompare-Objekt werden die Ergebnisse vom Aufruf der
    DataReference::compareElements - Funktionen graphisch dargestellt.
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class QtDialogCompare : public GuiQtForm, public DialogCompare {
 public:
  /// Konstruktor
  QtDialogCompare( const std::string& name );

  /// Destruktor
  ~QtDialogCompare();

  /*=============================================================================*/
  /* member functions from GuiElement                                            */
  /*=============================================================================*/

  void create();
  void manage();
  void map();
  void unmanage();
  void unmap();
  void update(GuiElement::UpdateReason);
  void serializeXML(std::ostream&, bool recursive = false) {}
  QWidget* myWidget();
  bool destroy();

  /*=============================================================================*/
  /* public member functions                                                     */
  /*=============================================================================*/

  static QtDialogCompare& Instance();

  /** Create-Methode
      @param xferNodes Liste aller zu dargestellenden XferDataItems
      @param rootLabel Liste aller RootLabels
  */
  void build(std::vector<XferDataItem*>& xferNode, const std::vector<std::string>& rootLabel);

  /** Create-Methode
      @param columnLabels Liste Labels der Spalten im Navigator
  */
  void setHeaders(const std::vector<std::string>& columnLabels);

  /** Starten des Dialog
   */
  void exec();

 private:
    // Klasse zur Verarbeitung von Ereignissen vom Button 'Close'
    class CloseButtonListener : public GuiButtonListener {
      public:
        CloseButtonListener( GuiQtForm *dialog ) : m_dialog( dialog ) {}
        virtual void ButtonPressed();
        virtual JobAction* getAction() { return 0; }
      private:
        GuiQtForm *m_dialog;
    };

    struct ItemData {
      ItemData(const ItemData& itemInfo, const std::string& lb)
      : label(itemInfo.label)
      , branch(itemInfo.branch)
      , xfer(0) {
	branch.push_back( lb );
      };

      ItemData(const ItemData& itemInfo, XferDataItem* xf)
      : label(itemInfo.label)
      , branch(itemInfo.branch)
	, xfer(xf) {
      };

      ItemData(const std::string& l, XferDataItem* x):label(l), xfer(x) {};

      std::string Label() const {
	std::string s;
	for (std::vector<std::string>::const_iterator it = branch.begin(); it != branch.end(); ++it)
	  s += ((s.size() && (*it)[0] != '[')? "." : "") + *it;
	return s;
      }

      std::vector<std::string> branch;
      XferDataItem* xfer;
      std::string label;
    };
    typedef std::vector<ItemData> ItemDataVector;

    /*=============================================================================*/
    /* member functions from QtDialogCompare                                   */
    /*=============================================================================*/

    void addStructNode(XferDataItem* node, const ItemData& itemInfo);
    void addStructItem(XferDataItem* node, int index, const ItemData& itemInfo);
    void addNode(XferDataItem* node, const ItemData& itemInfo, bool top=false);

    GuiFieldgroup* createScalarFieldGroup(const ItemDataVector& xferNodes, const std::string& title);
    GuiElement* createScalarTable(const ItemDataVector& xferNodes, const std::string& title);
    GuiElement* createScalarPlot(const ItemDataVector& xferNodes, const std::string& title);

    GuiElement* createVectorTable(const ItemDataVector& xferNodes, const std::string& title);
    GuiElement* createVectorPlot(const ItemDataVector& xferNodes, const std::string& title);

    GuiElement* createMatrixTab(const ItemDataVector& xferNodes, const std::string& title);
    void createOthersTab(const ItemDataVector& xferNodes, const std::string& title);
    bool extractSubNodes(ItemDataVector& allNodes, int* start,
			 ItemDataVector& subNodes, std::string& label);
    bool isInsideIgnoreList(XferDataItem* node);
    double getScaleFactor(const std::string& unit);

    /*=============================================================================*/
    /* member variables from QtDialogCompare                                    */
    /*=============================================================================*/

 private:
    ItemDataVector m_xferScalarNodes;
    ItemDataVector m_xferVectorNodes;
    ItemDataVector m_xferMatrixNodes;
    ItemDataVector m_xferOthersNodes;
    std::vector<Gui2dPlot*> m_plot2dVector;

    int        m_maxScalarCols;
    GuiQtForm* m_dialog;
    GuiOrientationContainer* m_scalarTab;
    GuiOrientationContainer* m_vectorTab;
    GuiOrientationContainer* m_matrixTab;
    GuiOrientationContainer* m_othersTab;
    std::vector<std::string> m_header;
    std::vector<std::string> m_itemIgnoreList;

    static QtDialogCompare* s_compareDialog;
};

#endif
