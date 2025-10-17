
#include <QtGui/QKeyEvent>
#include <QScrollBar>
#include <QHeaderView>
#include <QApplication>
#if QT_VERSION > 0x050600
#include <QScreen>
#else
#include <QDesktopWidget>
#endif
#include <QClipboard>
#include <QSettings>
#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QLabel>
#include <QDateTime>
#include <QRadioButton>
#include <QCheckBox>
#include <qdrawutil.h>
#include <QMimeData>
#include <QColorDialog>


#include "gui/GuiSeparator.h"
#include "gui/GuiTableItem.h"
#include "gui/GuiPopupMenu.h"
#include "gui/GuiMenuButton.h"
#include "gui/Timer.h"
#include "gui/DialogWorkClock.h"
#include "gui/qt/GuiQtTableViewBase.h"
#include "gui/qt/GuiQtList.h"
#include "gui/qt/GuiQtTable.h"
#include "gui/qt/GuiQtList.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtTableComboBoxItem.h"
#include "gui/qt/QtIconManager.h"
#include "app/DataPoolIntens.h"

#include "utils/gettext.h"
#include "utils/StringUtils.h"
#include <streamer/Stream.h>
#include "job/JobManager.h"

INIT_LOGGER();

const int GuiQtTableViewBase::COLUMN_MARGIN_WIDTH = 3;  // column increase pixelsize

/*=============================================================================*/
/* QStandardItemModel Derivation                                               */
/*=============================================================================*/
QtStandardItemModel::QtStandardItemModel(QObject* parent )
  : QStandardItemModel(parent)
  , m_table(dynamic_cast<GuiQtTable*>(parent))
  , m_list(dynamic_cast<GuiQtList*>(parent))
{
}
QtStandardItemModel::QtStandardItemModel(int rows, int columns, QObject* parent )
  : QStandardItemModel(rows, columns, parent)
  , m_table(dynamic_cast<GuiQtTable*>(parent))
  , m_list(dynamic_cast<GuiQtList*>(parent))
{
}

void QtStandardItemModel::beginResetModel() { QStandardItemModel::beginResetModel(); }
void QtStandardItemModel::endResetModel() { QStandardItemModel::endResetModel(); }

void QtStandardItemModel::signalDataChanged(const QModelIndex& tl, const QModelIndex& br) {
  emit dataChanged(tl, br);
}
QVariant QtStandardItemModel::data( const QModelIndex& index, int role) const {
  // is called with role:
  // 0 Qt::DisplayRole - format Date, DateTime and Time
  // 1 Qt::DecorationRole
  // 6 Qt::FontRole
  // 7 Qt::TextAlignmentRole - return correct Qt::Alignment
  // 8 Qt::BackgroundRole - return QBrush with background color if item is colorized (colorSet or colorBit)
  // 9 Qt::ForegroundRole - return QBrush with foreground color if item is colorized (colorSet or colorBit)
  // 10 Qt::CheckStateRole
  if (role == Qt::TextAlignmentRole) {
    DataDictionary::DataType dataType(DataDictionary::type_String);
    bool bInverted(false);
    // ui_field_alignment: < | > ^ _
    Qt::AlignmentFlag alignment((Qt::AlignmentFlag) 0);
    bool isCombobox(false);
    if (m_list) {
      if (m_list->col(index.column())) {
        m_list->col(index.column())->getDataType( dataType );
        bInverted = m_list->col(index.column())->isInverted();
        alignment = m_list->getQtAlignment(m_list->col(index.column())->getAlignment());
        // todo: set isCombobox
      }
    } else if (m_table) {
      GuiTableItem *item = m_table->getTableItem(index.row(), index.column());
      GuiQtDataField *dfItem = 0;
      if(item && (dfItem = dynamic_cast<GuiQtDataField*>(item->getDataField()))){
        dataType = item->getDataField()->getDataType();
        bInverted = item->getDataField()->isInverted();
        alignment = dfItem->getQtAlignment();
        isCombobox = item->getDataField()->Attr()->IsCombobox();
      }
    }
    if(alignment)
      return alignment;
    if (!isCombobox &&  // combobox is aligned as a STRING
        (dataType == DataDictionary::type_Real ||
         dataType == DataDictionary::type_Integer))
      return (bInverted ? Qt::AlignLeft : Qt::AlignRight);
    return (bInverted ? Qt::AlignRight : Qt::AlignLeft);
  }

  // special case button ==> empty data
  if (m_table) {
    GuiTableItem *item = m_table->getTableItem(index.row(), index.column());
    if(item && item->getDataField() && item->getDataField()->Attr()->IsButton())
      return QVariant();
  }

  QVariant var = QStandardItemModel::data(index, role);
  if (role == Qt::DisplayRole) {
#if QT_VERSION >= 0x060000
     if (var.metaType() == QMetaType(QMetaType::QDate)) {
      return QLocale::system().toString(var.toDate(), QLocale::ShortFormat);
    }
    if (var.metaType() == QMetaType(QMetaType::QDateTime)) {
      return QLocale::system().toString(var.toDateTime(), QLocale::ShortFormat);
    }
    if (var.metaType() == QMetaType(QMetaType::QTime)) {
      return QLocale::system().toString(var.toTime(), QLocale::ShortFormat);
    }
#else
     if (var.type() == QVariant::Date) {
      return QLocale::system().toString(var.toDate(), QLocale::ShortFormat);
    }
    if (var.type() == QVariant::DateTime) {
      return QLocale::system().toString(var.toDateTime(), QLocale::ShortFormat);
    }
    if (var.type() == QVariant::Time) {
      return QLocale::system().toString(var.toTime(), QLocale::ShortFormat);
    }
#endif
  }

  if (role == Qt::ForegroundRole || role == Qt::BackgroundRole) {
    GuiTableItem *item = !m_table ? 0 :
      m_table->getTableItem(index.row(), index.column());
    GuiQtDataField *dfItem = 0;
    if(item && (dfItem = dynamic_cast<GuiQtDataField*>(item->getDataField()))){
      if(dfItem->useColorSet() || dfItem->colorBitIsSet()){
        QColor background, foreground;
        int dark_fac;
        dfItem->getTheColor( background, foreground, dark_fac );
        if (!background.isValid() || !foreground.isValid())
          return QBrush();
        return QBrush(role == Qt::ForegroundRole ? foreground : background);
      }
    }
  }
  return var;
}
QModelIndex QtStandardItemModel::parent(const QModelIndex &child) const {
  if (child.row() >= child.model()->rowCount() ||
      child.column() >= child.model()->columnCount()) {
    return QModelIndex();
  }
  return QStandardItemModel::parent(child);
}

Qt::ItemFlags QtStandardItemModel::flags ( const QModelIndex& index ) const {
  if (index.row() >= index.model()->rowCount() ||
      index.column() >= index.model()->columnCount()) {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags flags = QStandardItemModel::flags(index);
  if (m_list) // wir sind in der Liste
    return flags;
  if (!m_table) {// wir sind im CycleDialog
    if (index.column() == 0) {
      flags ^= Qt::ItemIsEditable;
    }
    return flags;
  }

  GuiTableItem *item = m_table->getTableItem( index.row(), index.column() );
  if(item && item->getDataField() &&
     (item->getDataField()->Attr()->GuiKind() == UserAttr::gui_kind_button)) {
    flags ^= Qt::ItemIsSelectable;
  }
  if (item && item->getDataField())
    if (item->isEditable()) {
      flags |= Qt::ItemIsEditable;
    } else {
      flags ^= Qt::ItemIsEditable;
    }
  else {
    flags ^= Qt::ItemIsEditable;
    //      flags ^= Qt::ItemIsSelectable; //Qt::ItemIsUserCheckable;
  }
  if (item && item->ItemType() == GuiTableItem::item_Label){
    if (item->isEditable()){
      flags |= Qt::ItemIsEnabled;
    } else {
      flags ^= Qt::ItemIsEnabled;
    }
  }
  if (index.row() < m_table->getHLineList().size() &&
      index.column() < m_table->getVLineList().size()) {
    flags = Qt::NoItemFlags;
  }
  return flags;
}

void QtStandardItemModel::sort(int column, Qt::SortOrder order) {
  if (m_list) {
    m_list->sort(column, order == Qt::AscendingOrder ? 1 : -1);
  }
  QStandardItemModel::sort(column, order);
}

/*=============================================================================*/
/* SpreadSheetDelegate                                                         */
/*=============================================================================*/
SpreadSheetDelegate::SpreadSheetDelegate(GuiQtTable* table, QObject *parent)
  : QStyledItemDelegate(parent)
  , m_table(table)
  , m_list(0)
  , m_countDecimalPlace(-1)
  , m_mouseButtonPressed(false)
{}

SpreadSheetDelegate::SpreadSheetDelegate(GuiQtList* list, int countDecimalPlace, QObject *parent)
  : QStyledItemDelegate(parent)
  , m_table(0)
  , m_list(list)
  , m_countDecimalPlace(countDecimalPlace)
  , m_mouseButtonPressed(false)
{}

void SpreadSheetDelegate::MyQLineEdit::keyPressEvent(QKeyEvent* e) {
  // multiple selected cells, ignore this keyEvent
  if (m_table && m_table->selectedIndexes().size() > 1) {
    e->ignore();
    return;
  }
  if (DataPoolIntens::getDataPool().isUndoActivated()) {
    // Crtl+Z Undo
    if (e->key() == Qt::Key_Z && e->modifiers() == Qt::ControlModifier) {
      DataPoolIntens::getDataPool().undo();
      return;
    }
    // Crtl+Shift+Z Redo
    if (e->key() == Qt::Key_Y && e->modifiers() == Qt::ControlModifier) {
      DataPoolIntens::getDataPool().redo();
      return;
    }
  }
  QLineEdit::keyPressEvent(e);
}

void SpreadSheetDelegate::MyQComboBox::keyPressEvent(QKeyEvent* e) {
  // multiple selected cells, ignore this keyEvent
  if (m_table && m_table->selectedIndexes().size() > 1) {
    e->ignore();
    return;
  }
  if (DataPoolIntens::getDataPool().isUndoActivated()) {
    // Crtl+Z Undo
    if (e->key() == Qt::Key_Z && e->modifiers() == Qt::ControlModifier) {
      DataPoolIntens::getDataPool().undo();
      return;
    }
    // Crtl+Shift+Z Redo
    if (e->key() == Qt::Key_Y && e->modifiers() == Qt::ControlModifier) {
      DataPoolIntens::getDataPool().redo();
      return;
    }
  }
  QComboBox::keyPressEvent(e);
}

QWidget* SpreadSheetDelegate::createEditorWidget(GuiQtTable *table, GuiTableItem *item, bool activate,
                                                const QModelIndex& index, QWidget* parent, const QObject* obj) {
  QWidget *e=NULL;
  // create RadioButton
  if(item->getDataField()->Attr()->IsRadio()) {
    e = new QRadioButton(parent);
    e->setDisabled(true);
  }

  // create CheckBoxButton
  if(item->getDataField()->Attr()->IsToggle())
    e  = new QCheckBox("           ", parent);

  // commen settings for RadioButton & CheckBoxButton
  if (e) {
    connect(e,SIGNAL(clicked(bool)), obj, SLOT(buttonChecked(bool)));
    // successive a MouseButtonPress => toggle value
    if (activate) {
      item->ValidateCell( (index.model()->data(index, Qt::EditRole).toBool() ? "0" : "1") );
      table->m_tablewidget->model()->setData(index, index.model()->data(index, Qt::EditRole).toBool(), Qt::DisplayRole);
    }
  }

  // create comboBox
  if(item->getDataField()->Attr()->GuiKind() == UserAttr::gui_kind_combobox){
    QComboBox *editor = new MyQComboBox(parent, table->m_tablewidget);
    e = editor;
    GuiQtTableComboBoxItem *cb_item = dynamic_cast<GuiQtTableComboBoxItem*>(item);
    QStringList sl = cb_item->getCellItems();
    if (cb_item && sl.size() > 1) {
      cb_item->getTrimmedCellItems( sl );
      if (sl.size()) {
        editor->addItems( cb_item->getCellItems() );
        connect(editor,SIGNAL(activated(const QString&)), obj, SLOT(comboxBoxActivate(const QString&)));
      } else e=NULL; // Empty DataSet => create editLine
    } else e=NULL; // Empty DataSet => create editLine
    if (!e) delete editor;
  }

  // default LineEdit
  if (!e) {
    QLineEdit *le = new MyQLineEdit(parent, table->m_tablewidget);
    connect(le,SIGNAL(editingFinished()), table->m_tablewidget, SLOT(editingFinished()));
    connect(le,SIGNAL(returnPressed()), obj, SLOT(lineEditReturnPressed()));
    dynamic_cast<GuiQtDataField*>(item->getDataField())->setValidator( le );
    le->selectAll();
    e = le;
  }
  if (e)
    e->setObjectName("EditorWidget");

  return e;
}

QWidget *SpreadSheetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option,
    const QModelIndex &index) const
{
  GuiTableItem *item = m_table->getTableItem( index.row(), index.column() );
  if (!(item && item->getDataField() && item->isEditable()))
    return 0;
  return createEditorWidget(m_table, item, m_mouseButtonPressed, index, parent, this);
}

void SpreadSheetDelegate::buttonChecked(bool checked){
  QAbstractButton *editor = qobject_cast<QAbstractButton *>(sender());
  emit commitData(editor);
}
void SpreadSheetDelegate::comboxBoxActivate(const QString& str){
  QComboBox *editor = qobject_cast<QComboBox *>(sender());
  emit commitData(editor);
}
void SpreadSheetDelegate::lineEditReturnPressed(){
  QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
  emit commitData(editor);
}

bool SpreadSheetDelegate::editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index ) {
  // is createEditor Event successive a MouseButtonPress
  if (event->type() == QEvent::MouseButtonPress) {
    m_mouseButtonPressed = true;
  } else if (event->type() == QEvent::MouseButtonRelease) {
    m_mouseButtonPressed = false;
  }

  // try to fix qt bug
  // last edited editor widget is closed, but it seems to be selected.
  // in case of leaving QTableView and reentering into last edited Widget
  // this provide that is really possible
  if (event->type() == QEvent::MouseButtonPress &&
      m_table->m_tablewidget->currentIndex() == index) {
    bool blocked(signalsBlocked());
    m_table->m_tablewidget->blockSignals(true);
    m_table->m_tablewidget->setCurrentIndex(m_table->m_tablewidget->rootIndex());
    m_table->m_tablewidget->setCurrentIndex(index);
    m_table->m_tablewidget->blockSignals(blocked);
  }
  return QStyledItemDelegate::editorEvent(event, model, option, index);
}

bool SpreadSheetDelegate::eventFilter(QObject *editor, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    switch (static_cast<QKeyEvent *>(event)->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
      QEvent* myEvent = new QKeyEvent(event->type(),
				      Qt::Key_Tab,
				      dynamic_cast<QKeyEvent*>(event)->modifiers());
      bool ret = QStyledItemDelegate::eventFilter(editor, myEvent);
      delete myEvent;
      return ret;
    }
  }
  return QStyledItemDelegate::eventFilter(editor, event);
}


void SpreadSheetDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *edit = qobject_cast<QComboBox *>(editor);
    if (edit) {
      edit->setCurrentIndex( edit->findText(index.model()->data(index, Qt::EditRole).toString()) );
    } else
      {
        QAbstractButton *cb = qobject_cast<QAbstractButton *>(editor);
        if (cb) {
          cb->setChecked( index.model()->data(index, Qt::EditRole).toBool() );
        }
        else {
           QStyledItemDelegate::setEditorData( editor, index);
           // select all (this is not always the default behaviour (items with intens func))
           QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
           if (edit) edit->selectAll();
        }
      }
}

void SpreadSheetDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  QStyledItemDelegate::updateEditorGeometry( editor, option, index );

  // wegen Problemen mit dem RadioButton machen wir das selber
  QAbstractButton *cb = qobject_cast<QAbstractButton *>(editor);
  if (0&&cb) {
    double di = floor(0.5+(cb->size().width()/2.)-(cb->sizeHint().height()/2.));
    cb->move(cb->pos().x()+di,cb->pos().y());
    cb->setMaximumWidth( m_table->m_tablewidget->columnWidth(index.column())-di );
  }
}

void SpreadSheetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const
{
  GuiTableItem *item = m_table->getTableItem( index.row(), index.column() );
  if (!item) return;

  // normale QLineEdit
  QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
  if (edit) {
    // validator validation
    // if not pass, set old value
    if (edit->validator()) {
      int p=0;
      QString s = edit->text();
      std::string sOld;
      item->getDataField()->getValue(sOld);
      QValidator::State ret = edit->validator()->validate(s, p);
      if (!(s.size() == 0 || ret == QValidator::Acceptable)) {
        edit->setText( QString::fromStdString(sOld) );
        m_table->printMessage( _("Conversion error."),
                               GuiElement::msg_Warning );
        return;
      }
    }
    item->ValidateCell( edit->text().toStdString() );
    return;
  }

  // QComboBox
  QComboBox *cedit = qobject_cast<QComboBox *>(editor);
  if (cedit) {
    item->ValidateCell(  cedit->currentText().toStdString() );
    return;
  }

  // QRadioButton
  QAbstractButton *cb = qobject_cast<QAbstractButton *>(editor);
  if (cb) {
    // ignore "0" if dataField is invalid
    std::string s;
    if (!item->getDataField()->getValue(s) && !cb->isChecked())
      return;
    item->ValidateCell( (cb->isChecked() ? "1" : "0") );
    return;
  }

  // anderes
  else {
    QStyledItemDelegate::setModelData( editor, model, index);
    m_table->updateItem(index.row(), index.column(), item);
  }
}

QString SpreadSheetDelegate::displayText(const QVariant &value, const QLocale &locale) const {
#if QT_VERSION < 0x050000
  if (m_list) {
    if (value.userType() == QVariant::Double  && m_countDecimalPlace > 0) {
      // Sorting der Liste auch bei decimalPoint gleich Komma möglich
      return locale.toString(value.toDouble(), 'f', m_countDecimalPlace);
    }
  }
#endif
  return QStyledItemDelegate::displayText( value, locale);
}

void SpreadSheetDelegate::initStyleOption(QStyleOptionViewItem * option, const QModelIndex & index) const {
#if QT_VERSION > 0x050000
  if (m_list && index.isValid()) {
    std::string valueStr = m_list->col(index.column())->cellFormattedValue(index.row());
    option->text = QString::fromStdString(valueStr);
  }
#endif
  QStyledItemDelegate::initStyleOption(option, index);
}

void SpreadSheetDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option,
                                  const QModelIndex & index ) const {
  if (m_list) {
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }
  // unsichtbar machen (Hintergrundfarbe für ungenutzte (oben links) zellen)
  if (index.row() < m_table->getHLineList().size() &&
      index.column() < m_table->getVLineList().size() ) {
    painter->fillRect(option.rect, m_table->myWidget()->palette().color(QPalette::Window));
    return;
  }

  // set color
  QColor background, foreground;
  GuiTableItem *item = m_table->getTableItem( index.row(), index.column() );
  GuiQtDataField *dfItem=0;
  if (!item ||
      (item && !(dfItem = dynamic_cast<GuiQtDataField*>(item->getDataField() )))) {
    painter->fillRect(option.rect, m_table->myWidget()->palette().color(QPalette::Window));
    return;
  }
  if (dfItem){
    int dark_fac;
    dfItem->getTheColor( background, foreground, dark_fac );
  }
  if (!background.isValid() || !foreground.isValid()){
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }

  // make alternating background color
  if (!qApp->styleSheet().contains("QTableView")) {
    int ra = (index.row() % 4) < 2 ? 0 : index.row() % 4 -2;
    int rb = index.row() % 2;
    background = background.darker(100+5*rb);
    if (ra) {
      int blue = background.blue()+10*ra;
      if (blue < 256)
        background.setBlue( blue );
    }
    painter->fillRect(option.rect, background);
  }else if(dfItem &&
           (dfItem->useColorSet() || dfItem->colorBitIsSet())){
    painter->fillRect(option.rect, background);
  }

  // draw Combobox
  if (item->getDataField() && item->isEditable() &&
      item->getDataField()->Attr()->IsCombobox() ) {
    GuiQtTableComboBoxItem *cb_item = dynamic_cast<GuiQtTableComboBoxItem*>(item);
    if (cb_item) {
      QStringList sl = cb_item->getCellItems();
      if (sl.size() > 0) {
        cb_item->getTrimmedCellItems( sl );
        std::string str(item->CellValue());
        QString text = sl.contains(QString::fromStdString(str)) ?
          QString::fromStdString(str) : (str.length() > 0 ?
                                         QString::fromStdString(compose("(%1)", str)) : "");
#if QT_VERSION < 0x050000
        QStyleOptionViewItemV4 myOption = option;
#else
        QStyleOptionViewItem myOption = option;
#endif
        myOption.text = text;

        QStyleOptionComboBox comboBoxOption;
        if (!sl.contains(QString::fromStdString(str)) && str.length() > 0) {
          comboBoxOption.palette.setColor(QPalette::Button, QColor(Qt::red));
          comboBoxOption.palette.setColor(QPalette::Base, QColor(Qt::red));
          myOption.palette.setColor(QPalette::Button, QColor(Qt::red));
          myOption.palette.setColor(QPalette::Base, QColor(Qt::red));
          painter->fillRect(option.rect, QColor(Qt::red));
        } else {
          comboBoxOption.palette = option.palette;
        }
        comboBoxOption.rect = option.rect;
        comboBoxOption.state = option.state;
        comboBoxOption.state |= QStyle::State_Enabled;
        comboBoxOption.editable = AppData::Instance().GuiComboBoxEditable();
        comboBoxOption.currentText = text;
        QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &comboBoxOption, painter);
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
        //    QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &comboBoxOption, painter);
        return;
      }
    }
  }

  // draw RadioButton
  if( item->getDataField() &&
      (item->getDataField()->Attr()->IsToggle() ||
       item->getDataField()->Attr()->IsRadio()   ) ) {
    // keine Button?
    std::string s="btn Text";
    if (!dynamic_cast<GuiQtDataField*>(item->getDataField())->getValue(s))
      return;  // INVALID > don't show anything

    QStyleOptionButton BtnStyle;
    BtnStyle.palette = option.palette;
    if(dfItem && (dfItem->useColorSet() || dfItem->colorBitIsSet())){
      BtnStyle.palette.setColor(QPalette::Base, background);
      BtnStyle.palette.setColor(QPalette::Text, foreground);
    }

    BtnStyle.state = QStyle::State_Enabled;
    if(index.model()->data(index, Qt::DisplayRole).toBool() == true)
      BtnStyle.state |= QStyle::State_On;
    else
      BtnStyle.state |= QStyle::State_Off;
    Qt::AlignmentFlag alignment(dfItem->getQtAlignment());
    if (alignment ==  Qt::AlignRight)
      BtnStyle.direction = Qt::RightToLeft;
    else
      BtnStyle.direction = QApplication::layoutDirection();
    BtnStyle.rect = option.rect;
    if(alignment == Qt::AlignHCenter || alignment == Qt::AlignRight){
      double control_width = 18;  // todo: get correct width
      double factor = alignment == Qt::AlignHCenter ? 0.5 : 1.0;
      BtnStyle.rect.setX(BtnStyle.rect.x() +
                         (BtnStyle.rect.width() - control_width) * factor);
    }
    if (item->getDataField()->Attr()->IsToggle())
      QApplication::style()->drawControl(QStyle::CE_CheckBox,&BtnStyle,painter);
    else
      QApplication::style()->drawControl(QStyle::CE_RadioButton,&BtnStyle,painter);
    return;
  }

  // draw Button
  if( item->getDataField() && item->getDataField()->Attr()->IsButton()) {
    // keine Button?
    std::string s="btn Text";
    if (!dynamic_cast<GuiQtDataField*>(item->getDataField())->getValue(s))
      return;
    if (item->getDataField()->Attr()->IsColorPicker()) {
      QBrush brush( QColor(QString::fromStdString(s)) );
      qDrawWinButton(painter, option.rect.x()+2,option.rect.y()+1, option.rect.width()-4, option.rect.height()-2, m_table->myWidget()->palette(), false, &brush);
      return;
    }
    // trick to simulate press event
    GuiQtTableViewBase *tbl = m_table->myWidget()->findChild<GuiQtTableViewBase*>();
    int add = tbl->getPressedButton() == index ? 1 : 0;
    qDrawWinButton(painter, option.rect.x()+2+add,option.rect.y()+1+add, option.rect.width()-4-2*add, option.rect.height()-2-2*add, m_table->myWidget()->palette(), false, &m_table->myWidget()->palette().button());
    painter->drawText(option.rect.x()+2,option.rect.y()+1, option.rect.width()-4, option.rect.height()-2, Qt::AlignCenter, QString::fromStdString(s));
  }
  QStyledItemDelegate::paint(painter, option, index);
}

/*=============================================================================*/
/* QtHeaderView View Derivation                                                      */
/*=============================================================================*/

QtHeaderView::QtHeaderView ( GuiQtTable* tbl, Qt::Orientation orientation, QWidget * parent )
  : QHeaderView (orientation, parent)
  , m_table(tbl)
  , m_list(0) {
#if QT_VERSION < 0x050000
  setClickable(true);
#else
  setSectionsClickable(true);
#endif
}

QtHeaderView::QtHeaderView ( GuiQtList* list, Qt::Orientation orientation, QWidget * parent )
  : QHeaderView (orientation, parent)
  , m_table(0)
  , m_list(list) {
#if QT_VERSION < 0x050000
  setClickable(true);
#else
  setSectionsClickable(true);
#endif
}

void QtHeaderView::contextMenuEvent ( QContextMenuEvent * e ) {
  if (m_table) {
    if ( ! m_table->isEditable() || m_table->isIndexMenuHidden() ) return;
    GuiPopupMenu *menu = m_table->PopupMenu((int)e->x(), (int)e->y(),
                                            ((orientation() == Qt::Vertical) ?
                                             GuiElement::orient_Vertical :
                                             GuiElement::orient_Horizontal)
                                            );
    if (menu) {
      menu->getElement()->getQtElement()->myWidget()->move(e->globalX(), e->globalY());
      menu->getElement()->getQtElement()->myWidget()->show();
    }
  }
  if (m_list) {
    m_list->popup();
  }
}

void QtHeaderView::paintSection ( QPainter * painter, const QRect & rect, int logicalIndex ) const {
  if (m_table) {
    // die horizontalen zusätzlichen header texte schreiben wir selber
    if (orientation() == Qt::Horizontal &&
        logicalIndex < m_table->getVLineList().size()) {
      painter->fillRect(rect, m_table->myWidget()->palette().color(QPalette::Window));
      painter->drawText(rect, Qt::AlignCenter, model()->headerData(logicalIndex, orientation()).toString());
      return;
    }
    // die vertikale zusätzlichen header texte schreiben wir selber
    if (orientation() == Qt::Vertical &&
        logicalIndex < m_table->getHLineList().size()) {
      painter->fillRect(rect, m_table->myWidget()->palette().color(QPalette::Window));
      painter->drawText(rect, Qt::AlignCenter, model()->headerData(logicalIndex, orientation()).toString());
      return;
    }
  }
  QHeaderView::paintSection(painter, rect, logicalIndex);

}


/*=============================================================================*/
/* private Class CornerWidget                                                  */
/*=============================================================================*/
class  CornerWidget : public QLabel {
public:
  CornerWidget(GuiQtTableViewBase* tbl)
    :QLabel(" * "), m_table(tbl) {
    QPixmap icon;
    if( QtIconManager::Instance().getPixmap( "table_corner", icon ) ){
      setPixmap( icon );
    } else {
      QFont f = font();
      f.setPointSize(16);
      setFont( f );
    }
  }

  void contextMenuEvent(QContextMenuEvent *event) {
    if (m_table) {
      GuiPopupMenu *menu = m_table->getMenu();
      if (menu)
        menu->popup();
    }
  }
private:
  GuiQtTableViewBase* m_table;
};

/*=============================================================================*/
/* private Class   GuiQtTableViewBase                                          */
/*=============================================================================*/
GuiQtTableViewBase::GuiQtTableViewBase( QAbstractItemModel * model,
                                        GuiQtList* l, GuiQtTable* t,
                                        QHeaderView *horzHeader,
                                        QHeaderView *vertHeader)
  : m_list(l), m_table(t), m_blockSelection(false), m_blockScrollTo(false)
  , m_horz_sb_height(0), m_vert_sb_width(0)

  , m_menu(0)
  , m_menu_readonly(0)
  , m_copyAll( this, true )
  , m_copyAll_readonly( this, true )
  , m_copy( this, false )
  , m_copy_readonly( this, false )
  , m_paste( this, true )
  , m_clearSelected( this, false )
  , m_clearAll( this, true )
  , m_resetSorting( this )
  // , m_action(0)
  // , m_save_listener( this )
  , m_vertFrozenTableView(0)
  , m_horzFrozenTableView(0)
  , m_vertHorzFrozenTableView(0)
  , m_dialog(0)
  , m_timerId(0)
{
  setModel(model);
  if (horzHeader)
    setHorizontalHeader(horzHeader);
  if (vertHeader)
    setVerticalHeader(vertHeader);
  setEditTriggers ( QAbstractItemView::CurrentChanged);
  //  setCornerButtonEnabled(false);
  // verticalHeader()->setStretchLastSection(true);
  // horizontalHeader()->setStretchLastSection(true);

  // set corner widget to get control over own popup menu
  setCornerWidget( new CornerWidget(this) );

  // initialize frozen parts
  if ( m_table ) {
    if (m_table->getHLineList().size() ) {
      m_horzFrozenTableView = new QTableView(this);
    }
    if ( m_table->getVLineList().size() ) {
      m_vertFrozenTableView = new QTableView(this);
    }
    // wir brauchen noch ein vertHorzFrozenTableView
    if ( m_table->getHLineList().size() && m_table->getVLineList().size() ) {
      m_vertHorzFrozenTableView = new QTableView(this);
    }
    if ( m_table->getHLineList().size() || m_table->getVLineList().size() ) {
      init();
    }
  }

  //connect the headers and scrollbars of both tableviews together
  connect(horizontalHeader(),SIGNAL(sectionResized(int,int,int)), this,
          SLOT(updateSectionWidth(int,int,int)));
  connect(verticalHeader(),SIGNAL(sectionResized(int,int,int)), this,
          SLOT(updateSectionHeight(int,int,int)));
  if (m_vertFrozenTableView) {
    connect(m_vertFrozenTableView->verticalScrollBar(), SIGNAL(valueChanged(int)),
            verticalScrollBar(), SLOT(setValue(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
            m_vertFrozenTableView->verticalScrollBar(), SLOT(setValue(int)));
  }
  if (m_horzFrozenTableView) {
    connect(m_horzFrozenTableView->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            horizontalScrollBar(), SLOT(setValue(int)));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
            m_horzFrozenTableView->horizontalScrollBar(), SLOT(setValue(int)));
  }

  // create timer and task
  m_timer=GuiFactory::Instance()->createTimer(0);
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );
}

GuiQtTableViewBase::~GuiQtTableViewBase() {
  delete m_vertFrozenTableView;
  delete m_horzFrozenTableView;
  delete m_vertHorzFrozenTableView;
  if (m_timer) delete m_timer;
  if (m_task)  delete m_task;
}

void GuiQtTableViewBase::setAllRowHeight(int row, int height) {
  if (row == -1) {
    if (m_vertFrozenTableView)
      m_vertFrozenTableView->horizontalHeader()->setMinimumHeight(height);
    else {
      horizontalHeader()->setMinimumHeight( height );
    }
    return;
  }

  QTableView::setRowHeight(row, height);
  if (m_horzFrozenTableView && row >= 0 && row < m_table->getHLineList().size()) {
    m_horzFrozenTableView->setRowHeight(row, height);
    if (m_vertHorzFrozenTableView)
      m_vertHorzFrozenTableView->setRowHeight(row, height);
  }
  if (m_vertFrozenTableView) {
    m_vertFrozenTableView->setRowHeight(row, height);
  }
}

void GuiQtTableViewBase::updateFrozenParts() {
  // vertical frozen table :: set all unused column to hidden
  if (m_vertFrozenTableView) {
    int col = m_table->getVLineList().size();
    for (; col<model()->columnCount(); ++col) {
      m_vertFrozenTableView->setColumnHidden(col, true);
      if (m_vertHorzFrozenTableView)
        m_vertHorzFrozenTableView->setColumnHidden(col, true);
    }
    if( m_table->getVertLinePlacement() == GuiElement::align_Right ) {
      int l=0;
      for (int c=0; c<m_table->getVLineList().size(); ++c) {
        l +=  m_vertFrozenTableView->columnWidth(c);
      }
      setColumnWidth(model()->columnCount()-1, l);
    }
  }

  // horizontal frozen table :: set all unused row to hidden
  if (m_horzFrozenTableView) {
    int row =  m_table->getHLineList().size();
    for (; row<model()->rowCount(); ++row)
      m_horzFrozenTableView->setRowHidden(row, true);
  }

  // set header height of vertical frozen table
  if (m_vertFrozenTableView) {
    for (int row=0; row<model()->rowCount(); ++row) {
      m_vertFrozenTableView->setRowHeight( row, rowHeight(row) );
    }

    int max = m_vertFrozenTableView ? m_vertFrozenTableView->horizontalHeader()->height() : horizontalHeader()->height();
    horizontalHeader()->setMinimumHeight( max );
    horizontalHeader()->setMaximumHeight( max );
    if (horizontalHeader()->height() == max) {
      m_vertFrozenTableView->horizontalHeader()->setMinimumHeight( max );
      m_vertFrozenTableView->horizontalHeader()->setMaximumHeight( max );
    }
  }

  // set header width of horizontal frozen table
  if (m_horzFrozenTableView) {
    for (int col=m_table->getVLineList().size(); col<model()->columnCount(); ++col)
      m_horzFrozenTableView->setColumnWidth( col, columnWidth(col) );

    int max = std::max(verticalHeader()->width(),  m_horzFrozenTableView->verticalHeader()->width());
    verticalHeader()->setMinimumWidth( max );
    verticalHeader()->setMaximumWidth( max );
    if (verticalHeader()->width() == max) {
      m_horzFrozenTableView->verticalHeader()->setMinimumWidth( max );
      m_horzFrozenTableView->verticalHeader()->setMaximumWidth( max );
    }
  }
  updateFrozenTableGeometry();
}

bool GuiQtTableViewBase::isFrozenTablePart(const QModelIndex & index) {
  // !! Anmerkung: Vertikaler Spalten sind Teil der Haupttabelle
  if (index.row() < m_table->getHLineList().size()) {
    return true;
  }
#if 1
  if (index.column() < m_table->getVLineList().size()) {
    return true;
  }
#endif
  return false;
}

void GuiQtTableViewBase::init() {
  // initialize process vertical horizontal frozen table
  if (m_vertHorzFrozenTableView) {
    //    m_vertHorzFrozenTableView->setFocusPolicy(Qt::NoFocus);
    m_vertHorzFrozenTableView->setDisabled(true);
    m_vertHorzFrozenTableView->setModel(model());
    m_vertHorzFrozenTableView->verticalHeader()->hide();
    m_vertHorzFrozenTableView->horizontalHeader()->hide();
    m_vertHorzFrozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_vertHorzFrozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_vertHorzFrozenTableView->setStyleSheet(m_vertHorzFrozenTableView->styleSheet() + "\nQTableView { border: none; }");
  }

  // initialize process vertical frozen columns
  if (m_vertFrozenTableView)  {
    m_vertFrozenTableView->setModel(model());
    //    m_vertFrozenTableView->setFocusPolicy(Qt::NoFocus);
    m_vertFrozenTableView->verticalHeader()->hide();
    if(m_table->isTableColHeaderHidden())
      m_vertFrozenTableView->horizontalHeader()->hide();

#if QT_VERSION < 0x050000
    m_vertFrozenTableView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#else
    m_vertFrozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#endif
    viewport()->stackUnder(m_vertFrozenTableView);

    m_vertFrozenTableView->setStyleSheet(m_vertFrozenTableView->styleSheet() + "\nQTableView { border: none; }");
    m_vertFrozenTableView->setSelectionModel(selectionModel());
    int col = 0;
    for (; col < m_table->getVLineList().size(); ++col) {
      m_vertFrozenTableView->setColumnWidth( col, columnWidth(col) );
      if (m_vertHorzFrozenTableView)
        m_vertHorzFrozenTableView->setColumnWidth( col, columnWidth(col) );
    }
    for (; col<model()->columnCount(); ++col) {
      m_vertFrozenTableView->setColumnHidden(col, true);
      if (m_vertHorzFrozenTableView)
        m_vertHorzFrozenTableView->setColumnHidden(col, true);
    }

    m_vertFrozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_vertFrozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    updateFrozenTableGeometry();

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    m_vertFrozenTableView->setVerticalScrollMode(ScrollPerPixel);
    // m_vertHorzFrozenTableView->setVerticalScrollMode(ScrollPerPixel);
  }
  // initialize process horizontal frozen rows
  if (m_horzFrozenTableView)  {
    m_horzFrozenTableView->setModel(model());
    //    m_horzFrozenTableView->setFocusPolicy(Qt::NoFocus);
    m_horzFrozenTableView->horizontalHeader()->hide();
    if(m_table->isTableRowHeaderHidden())
      m_horzFrozenTableView->verticalHeader()->hide();
#if QT_VERSION < 0x050000
    m_horzFrozenTableView->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#else
    m_horzFrozenTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#endif
    m_horzFrozenTableView->verticalScrollBar()->setDisabled( true );
    viewport()->stackUnder(m_horzFrozenTableView);

    m_horzFrozenTableView->setStyleSheet(m_horzFrozenTableView->styleSheet() + "\nQTableView { border: none; }");
    m_horzFrozenTableView->setSelectionModel(selectionModel());
    int col = 0;
    for (; col < m_table->getHLineList().size(); ++col) {
      m_horzFrozenTableView->setRowHeight( col, rowHeight(col) );
      if (m_vertFrozenTableView)
        m_vertFrozenTableView->setRowHeight( col, rowHeight(col) );
      if (m_vertHorzFrozenTableView)
        m_vertHorzFrozenTableView->setRowHeight( col, rowHeight(col) );
    }
    for (; col<model()->rowCount(); ++col) {
      m_horzFrozenTableView->setRowHidden(col, true);
      if (m_vertHorzFrozenTableView)
        m_vertHorzFrozenTableView->setRowHidden(col, true);
    }

    m_horzFrozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_horzFrozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    updateFrozenTableGeometry();

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    m_horzFrozenTableView->setHorizontalScrollMode(ScrollPerPixel);
  }
}

void GuiQtTableViewBase::getBounds(long& cntValidRow, long& cntValidColumn) {
  cntValidRow = 0;
  cntValidColumn = 0;
  for (long r = 0; r < model()->rowCount(); ++r) {
    for (long c = 0; c < model()->columnCount(); ++c) {
      QVariant data = model()->data(model()->index(r, c));
      if (data.isValid() && data.toString().trimmed().length() > 0) {
        cntValidRow = std::max(cntValidRow, r+1);
        cntValidColumn = std::max(cntValidColumn, c+1);
      }
    }
  }
}

void GuiQtTableViewBase::postInit() {
  QRect rect(contentsRect());
  // post process vertical frozen columns
  if( m_table->getVertLinePlacement() == GuiElement::align_Right ) {
    for (int col=0; col < m_table->getVLineList().size(); ++col) {
      setColumnHidden(col, true);
      if (m_horzFrozenTableView) {
        m_horzFrozenTableView->setColumnHidden(col, true);
      }
    }
  }

  // post process horizontal frozen rows
  if (m_table->getHorzLinePlacement() == GuiElement::align_Bottom)
    for (int col=0; col < m_table->getHLineList().size(); ++col)
      setRowHidden(col, true);
}

void GuiQtTableViewBase::updateFrozenTableGeometry() {
  int wOff=0;
  int xOff=0;
  if (m_vertFrozenTableView) {
    //    int wOff=0;
    for (int col=0; col < m_table->getVLineList().size(); col++)
      wOff += m_vertFrozenTableView->columnWidth(col);
    if( m_table->getVertLinePlacement() == GuiElement::align_Left ) {
      m_vertFrozenTableView->setGeometry( verticalHeader()->width()+frameWidth(),
                                          frameWidth(), wOff,
                                          viewport()->height()+horizontalHeader()->height());
    } else {
      xOff = viewport()->width()-wOff;
      m_vertFrozenTableView->setGeometry( viewport()->width()+verticalHeader()->width()+frameWidth()-wOff,
                                          frameWidth(), wOff,
                                          viewport()->height()+horizontalHeader()->height());
    }
  }

  // horizontal frozen rows
  int hOff=0, yOff=0;

#if QT_VERSION > QT_VERSION_CHECK(5,14,0)
#endif
  if (m_horzFrozenTableView) {
    for (int row=0; row < m_table->getHLineList().size(); ++row)
      hOff += rowHeight(row);
    if( m_table->getHorzLinePlacement() == GuiElement::align_Top ){
      m_horzFrozenTableView->setGeometry(0,
                                         horizontalHeader()->height(),
                                         viewport()->width()+verticalHeader()->width() + 1.5*frameWidth(),
                                         hOff+1.5*frameWidth());
    } else {
      yOff = viewport()->height()+-hOff;
      m_horzFrozenTableView->setGeometry(0,
                                         horizontalHeader()->height()+frameWidth()+viewport()->height()-hOff,
                                         viewport()->width()+verticalHeader()->width(), hOff);
    }
  }
  // horizontal vertical frozen table
  if (m_vertHorzFrozenTableView) {
    m_vertHorzFrozenTableView->setGeometry(verticalHeader()->width()+xOff,
                                           horizontalHeader()->height()+yOff,
                                           wOff+1.5*frameWidth(),
                                           hOff+1.5*frameWidth());
  }
}

void GuiQtTableViewBase::updateSectionWidth(int logicalIndex, int xx, int newSize) {
  if (!m_table) return;

  if(logicalIndex<m_table->getVLineList().size()){
    if (m_vertFrozenTableView) {
      if (newSize) {
        m_vertFrozenTableView->setColumnWidth(logicalIndex,newSize);
        if (m_vertHorzFrozenTableView)
          m_vertHorzFrozenTableView->setColumnWidth( logicalIndex,newSize );
        updateFrozenTableGeometry();
      }
    }
  }
  if (m_horzFrozenTableView) {
    if (newSize) {
      m_horzFrozenTableView->setColumnWidth(logicalIndex,newSize);
      updateFrozenTableGeometry();
    }
  }
}

void GuiQtTableViewBase::updateSectionHeight(int logicalIndex, int, int newSize) {
  if (m_vertFrozenTableView)
    m_vertFrozenTableView->setRowHeight(logicalIndex, newSize);
  if (m_horzFrozenTableView)
    if (newSize ||logicalIndex <m_table->getHLineList().size()) {
      m_horzFrozenTableView->setRowHeight(logicalIndex, newSize);
      if (m_vertHorzFrozenTableView)
        m_vertHorzFrozenTableView->setRowHeight(logicalIndex, newSize);
    }
}

void GuiQtTableViewBase::nextEditableIndex(int& r, int& c, bool vert) {
  GuiTableItem *item;
  int first_r(r), first_c(c);
  do {
    if (vert) {
      int r_min = 0; // m_table->getHLineList().size();
      r = (r+1) < model()->rowCount() ? r+1 : r_min;
      c = r != r_min ? c : (c+1) < model()->columnCount() ? c+1 : 0;
    } else {
      int c_min = 0; // m_table->getVLineList().size();
      c = (c+1) < model()->columnCount() ? c+1 : 0;
      r = c > 0 ? r : (r+1) < model()->rowCount() ? r+1 : 0;
    }
    if (first_r == r && first_c == c) {
      // abort, loop completed
      m_nextEditable = QModelIndex(); // reset
      r = -1;
      c = -1;
      return;
    }
    m_nextEditable = currentIndex().sibling(r,c);
    item = m_table->getTableItem(m_nextEditable.row(), m_nextEditable.column());
    if (c <  m_table->getVLineList().size() && r < m_table->getHLineList().size()) continue;
  } while (!(item && item->isEditable()));
}

void GuiQtTableViewBase::editingFinished() {
  if (signalsBlocked()) return;
  QLineEdit *ledit = qobject_cast<QLineEdit *>(sender());

  // if navigation is Vertical, override horizontal navigation
  if (m_table->getNavigation() == GuiElement::orient_Vertical && currentIndex().isValid()) {
    GuiTableItem *item;
    int r = currentIndex().row();
    int c = currentIndex().column();
    bool vert = m_table->getNavigation() == GuiElement::orient_Vertical;
    nextEditableIndex(r,c, vert);
  }
}

void GuiQtTableViewBase::comboxBoxActivate(const QString& t) {
  QComboBox *cedit = qobject_cast<QComboBox *>(sender());
  GuiTableItem *item = m_table->getTableItem(currentIndex().row(), currentIndex().column());
  if (item && cedit) {
    item->ValidateCell(  cedit->currentText().toStdString() );
  }
}

void GuiQtTableViewBase::buttonChecked(bool checked) {
  QAbstractButton *bedit = qobject_cast<QAbstractButton*>(sender());
  GuiTableItem *item = m_table->getTableItem(currentIndex().row(), currentIndex().column());
  if (item && bedit) {
    item->ValidateCell(bedit->isChecked() ? "1" : "0");
  }
}

void GuiQtTableViewBase::lineEditReturnPressed(){
  // etwas besseres habe ich nicht gefunden
  clearSelection();
  return;
}

void GuiQtTableViewBase::resizeEvent(QResizeEvent * event) {
  QTableView::resizeEvent(event);
  updateFrozenTableGeometry();
}

QModelIndex GuiQtTableViewBase::moveCursor(CursorAction cursorAction,
                                           Qt::KeyboardModifiers modifiers) {
  QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

  if (m_vertFrozenTableView) {
    int wOff=0;
    for (int col=0; col < m_table->getVLineList().size(); col++)
      wOff += m_vertFrozenTableView->columnWidth(col);
    if(cursorAction == MoveLeft && current.column()>0
       && visualRect(current).topLeft().x() < wOff ){

      const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x()
        - wOff;
      horizontalScrollBar()->setValue(newValue);
    }
  }
  return current;
}

void GuiQtTableViewBase::setSectionHeight(int height) {
  verticalHeader()->setDefaultSectionSize(height);
  verticalHeader()->setMinimumSectionSize(height);
  if (m_horzFrozenTableView) {
    m_horzFrozenTableView->verticalHeader()->setDefaultSectionSize(height);
    m_horzFrozenTableView->verticalHeader()->setMinimumSectionSize(height);
  }
  if (m_vertFrozenTableView) {
    m_vertFrozenTableView->verticalHeader()->setDefaultSectionSize(height);
    m_vertFrozenTableView->verticalHeader()->setMinimumSectionSize(height);
  }
  if (m_vertHorzFrozenTableView) {
    m_vertHorzFrozenTableView->verticalHeader()->setDefaultSectionSize(height);
    m_vertHorzFrozenTableView->verticalHeader()->setMinimumSectionSize(height);
  }
}

/** makes frozen column happy */
void GuiQtTableViewBase::setFont( const QFont& font ) {
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
  QString qss = QString("\nQWidget{font-family: %1;font-size: %2pt;font-weight: %3;};")
    .arg(font.family())
    //    .arg(font.style() == QFont::StyleNormal ? "normal" : font.style() == QFont::StyleItalic ? "italic" : "oblique")
    .arg(font.pointSize())
    .arg(font.weight() == QFont::Normal ? "normal" : "bold");
  // std::cout << "qss["<<qss.toStdString()<<"]\n";
  QTableView::setStyleSheet(styleSheet() + qss);
  if (m_vertFrozenTableView) m_vertFrozenTableView->setStyleSheet(m_vertFrozenTableView->styleSheet() + qss);
  if (m_horzFrozenTableView) m_horzFrozenTableView->setStyleSheet(styleSheet() + qss);
  if (m_vertHorzFrozenTableView) m_vertHorzFrozenTableView->setStyleSheet(styleSheet() + qss);
#else
  QTableView::setFont(font);
  if (m_vertFrozenTableView) m_vertFrozenTableView->setFont(font);
  if (m_horzFrozenTableView) m_horzFrozenTableView->setFont(font);
  if (m_vertHorzFrozenTableView) m_vertHorzFrozenTableView->setFont(font);
#endif
}

void GuiQtTableViewBase::setHeaderFont( const QFont& font ) {
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
  QString qss = QString("\nQHeaderView{font-family: %1;;font-size: %2pt;font-weight: %3;};")
    .arg(font.family())
    //    .arg(font.style() == QFont::StyleNormal ? "normal" : font.style() == QFont::StyleItalic ? "italic" : "oblique")
    .arg(font.pointSize())
    .arg(font.weight() == QFont::Normal ? "normal" : "bold");
  // std::cout << "Qss["<<qss.toStdString()<<"]\n";
  horizontalHeader()->setStyleSheet(horizontalHeader()->styleSheet() + qss);
  verticalHeader()->setStyleSheet(verticalHeader()->styleSheet() + qss);
  if (m_vertFrozenTableView) m_vertFrozenTableView->horizontalHeader()->setStyleSheet(qss);
  if (m_horzFrozenTableView) m_horzFrozenTableView->verticalHeader()->setStyleSheet(qss);
#else
  horizontalHeader()->setFont(font);
  verticalHeader()->setFont(font);
  if (m_vertFrozenTableView) m_vertFrozenTableView->horizontalHeader()->setFont(font);
  if (m_horzFrozenTableView) m_horzFrozenTableView->verticalHeader()->setFont(font);
#endif
}

 void GuiQtTableViewBase::setItemDelegate ( QAbstractItemDelegate * delegate ) {
  if (m_vertFrozenTableView) m_vertFrozenTableView->setItemDelegate(delegate);
  if (m_horzFrozenTableView) m_horzFrozenTableView->setItemDelegate(delegate);
  if (m_vertHorzFrozenTableView) m_vertHorzFrozenTableView->setItemDelegate(delegate);
  QTableView::setItemDelegate(delegate);
}

void GuiQtTableViewBase::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
  try{
    ///    std::cout << "!! CurChanged["<<current.row()<<", "<<current.column()<<"] <= ["<<previous.row()<<", "<<previous.column()<<"] Valid["<<current.isValid()<<", "<<previous.isValid()<<"] Name["<<m_table->getName()<<"] m_nextEditable  VALID["<<m_nextEditable .isValid()<<"]\n";
    if (m_table) {
      if (m_nextEditable .isValid()) {
        clearSelection();
        QModelIndex next = current.sibling(m_nextEditable.row(), m_nextEditable.column());
        m_nextEditable = QModelIndex(); // reset
        QTableView::currentChanged(next, previous);
        setCurrentIndex(next);
        return;
      }
      GuiTableItem *item = m_table->getTableItem( current.row(), current.column() );
      if (item && item->getDataField() && item->isEditable()) {
        QTableView::currentChanged(current, previous);
      } else {
        QTableView::currentChanged(current, previous);
        clearSelection();
        if (previous.isValid()) {
          bool vert = m_table->getNavigation() == GuiElement::orient_Vertical;
          int r = previous.row();
          int c = previous.column();
          nextEditableIndex(r, c, vert);
          m_timerId = startTimer(0);  // setCurrentIndex(m_nextEditable) by Timer
        }
      }
    }
  }  catch (std::exception ) {
  }
}

//----------------------------------------------------
// timerEvent
//----------------------------------------------------
void GuiQtTableViewBase::timerEvent(QTimerEvent * event) {
  if (m_timerId) {
    if (m_nextEditable.isValid()) {
      if (selectedIndexes().size() == 0) {
        setCurrentIndex(m_nextEditable);
      }
      m_nextEditable = QModelIndex(); // reset
    }
    killTimer(m_timerId);
    m_timerId = 0;
  }
  QTableView::timerEvent(event);
}

//----------------------------------------------------
// leaveEvent
//----------------------------------------------------
void GuiQtTableViewBase::leaveEvent(QEvent * event) {
  QTableView::leaveEvent(event);
}

void GuiQtTableViewBase::keyPressEvent ( QKeyEvent * e ) {
  if (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab ) {
    e->ignore();
    return;
  }
  // Crtl+C  Copy
  if (e->key() == Qt::Key_C && e->modifiers() == Qt::ControlModifier) {
    copy(false);
    return;
  }
  // Crtl+V  Paste
  if (e->key() == Qt::Key_V && e->modifiers() == Qt::ControlModifier) {
    paste();
    return;
  }
  // Crtl+U Unselect
  if (e->key() == Qt::Key_U && e->modifiers() == Qt::ControlModifier) {
    clearSelection();
    return;
  }
  // Crtl+A select all
  if (e->key() == Qt::Key_A && e->modifiers() == Qt::ControlModifier) {
    selectAll();
    return;
  }
  QTableView::keyPressEvent(e);
}

// add a blocked selectRow method
void GuiQtTableViewBase::blocked_selectRows ( std::vector<int> rows ) {
  m_blockSelection = true;
  SelectionMode sm = selectionMode();
  SelectionBehavior sb = selectionBehavior();
  if (m_table) {
    setSelectionMode( QTableView::ExtendedSelection );
    setSelectionBehavior ( QAbstractItemView::SelectRows );
  } else
    setSelectionMode( QTableView::MultiSelection );
  clearSelection();
  std::vector<int>::iterator it = rows.begin();
  for (;it != rows.end(); ++it) {
    QTableView::selectRow((*it));
  }
  setSelectionMode( sm );
  setSelectionBehavior( sb );
  m_blockSelection = false;
}
// add a blocked selectRow method
void GuiQtTableViewBase::blocked_selectRow ( int row ) {
  m_blockSelection = true;
  QTableView::selectRow(row);
  m_blockSelection = false;
}

// add a blocked selectRow method
void GuiQtTableViewBase::scroll_blocked_selectRow ( int row ) {
  m_blockScrollTo = true;
  blocked_selectRow(row);
  m_blockScrollTo = false;
}

// add a blocked selectRow method
void GuiQtTableViewBase::scroll_blocked_selectRows ( std::vector<int> rows) {
  m_blockScrollTo = true;
  blocked_selectRows(rows);
  m_blockScrollTo = false;
}

void GuiQtTableViewBase::selectAll() {
return;
  long cntValidRow(0), cntValidColumn;
  getBounds(cntValidRow, cntValidColumn);

  // select only valid data (from 0,0 to cntValidRow-1, cntValidColumn-1)
  const QItemSelection selection(model()->index(0, 0), model()->index(cntValidRow-1, cntValidColumn-1));
  QRegion region = visualRegionForSelection(selection);
  setSelection(region.boundingRect(), QItemSelectionModel::Select);
}

void GuiQtTableViewBase::selectionChanged( const QItemSelection & selected,
                                           const QItemSelection & deselected )  {
  QTableView::selectionChanged(selected, deselected);
  if (m_blockSelection) return;

  if (m_table) {
    if (deselected.indexes().size()) {
      m_table->rowUnselect(deselected.indexes().first());
    }
    if (selected.indexes().size())
      m_table->rowSelect(selected.indexes().first());
  } else {
    if (deselected.indexes().size()) {
      m_list->rowUnselect(deselected.indexes().first());
    }
    if (selected.indexes().size())
      m_list->rowSelect(selected.indexes().first());
  }
}

QModelIndexList GuiQtTableViewBase::selectedIndexes() const {
  int x0, y0, x1, y1;
  if (AppData::Instance().HeadlessWebMode() &&
      m_table && m_table->popArgSelected(x0, y0, x1, y1) ||
      m_list && m_list->popArgSelected(x0, y0, x1, y1)) {
    QModelIndexList selIdxs;
    QModelIndex idx;
    if (x0 < 0 || y0 < 0) { // invalid selection
      return selIdxs;
    }
    if (x1 < 0) x1 = x0;
    if (y1 < 0) y1 = y0;
    for (int x=x0; x<=x1; ++x) {
      for (int y=y0; y<=y1; ++y) {
        selIdxs.append( model()->index(x, y) );
      }
    }
    BUG_DEBUG("(Pop) selectedIndexes size:" << selIdxs.size());
    return selIdxs;
  }
  return QTableView::selectedIndexes();
}

int GuiQtTableViewBase::sizeHintForRow(int row) const {
  return QTableView::sizeHintForRow(row)+2;
}

void GuiQtTableViewBase::paintEvent ( QPaintEvent * event )  {
  QTableView::paintEvent(event);
}

void GuiQtTableViewBase::scrollTo(const QModelIndex &index, ScrollHint sh) {
  // TABLE
  if (m_table) {
    if(!m_vertFrozenTableView || index.column()>0)  /* wegen frozen??? */
        QTableView::scrollTo(index, sh);
    return;
  }

  // LIST
  if (m_blockScrollTo) return;

  // only scroll if selected row is not visible (completely)
  // scroll as little as possible
  if(rowViewportPosition(index.row()) < 0) {
    verticalScrollBar()->setValue(index.row());
  } else {
    int rowHeight = rowViewportPosition(1) - rowViewportPosition(0) - 1;  // 1: border
    if(rowViewportPosition(index.row()) > viewport()->height() - rowHeight) {
      int visibleTableRows = (viewport()->height() + 1) / (rowHeight + 1);  // 1: border
      int val = 1 + index.row() - visibleTableRows;
      verticalScrollBar()->setValue(val);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* minimumSizeHint -- used from GuiQtTable                                     */
/* --------------------------------------------------------------------------- */
QSize GuiQtTableViewBase::minimumSizeHint() const{
  return QTableView::minimumSizeHint();
}

/* --------------------------------------------------------------------------- */
/* contextMenuEvent -- used from GuiQtTable                                     */
/* --------------------------------------------------------------------------- */
void GuiQtTableViewBase::contextMenuEvent(QContextMenuEvent *event) {
  if (m_list)
    m_list->popup();
}

/* --------------------------------------------------------------------------- */
/*  sizeHint -- used from GuiQtTable                                           */
/* --------------------------------------------------------------------------- */
QSize GuiQtTableViewBase::sizeHint () const {
  QSize minhs;
  QSize windowSize = window()->size();
#if QT_VERSION > 0x050600
  QRect drect = QGuiApplication::primaryScreen()->availableGeometry();
#else
  QRect drect = QApplication::desktop()->availableGeometry();
#endif

  if ( m_hs.isValid() ) {
    bool hsb = false, vsb=false;
    if (drect.width() >= windowSize.width()) {
      minhs.setWidth(m_hs.width());
    } else {
      minhs.setWidth( drect.width() - abs(m_hs.width()-windowSize.width()) ) ;
      hsb = true;
    }
    if (drect.height() >= windowSize.height()) {
      minhs.setHeight( m_hs.height() );
    } else {
      minhs.setHeight( drect.height() - abs(m_hs.height()-windowSize.height())/*-8*/);
      vsb = true;
    }
    if (!m_horz_sb_height && hsb) {
      minhs.setHeight( minhs.height() + horizontalScrollBar()->height() );
    }
    if (!m_vert_sb_width && vsb) {
      minhs.setWidth( minhs.width() + verticalScrollBar()->width() );
    }
  }

  // append to window size
  if(window()) {
    static bool block=false;
    if (block) {
      return minhs;
    }
    block = true;
#if QT_VERSION > 0x050600
    QRect desktopRect = QGuiApplication::primaryScreen()->availableGeometry();
#else
    QRect desktopRect = QApplication::desktop()->availableGeometry();
#endif
    QSize winSH = window()->sizeHint();
    block = false;
    if (winSH.width() > desktopRect.width())
      minhs.setWidth( minhs.width() - (winSH.width()-desktopRect.width()) );
    if (winSH.height() > desktopRect.height())
      minhs.setHeight( minhs.height() - (winSH.height()-desktopRect.height()) );

    // nochmals korrigieren
    winSH = window()->sizeHint();
    if (winSH.height() > desktopRect.height() )
      if (winSH.height()>minhs.height())
        minhs.setHeight( minhs.height() - (winSH.height()-desktopRect.height()) );
    if (winSH.width() > desktopRect.width() )
      if (winSH.width()>minhs.width())
        minhs.setWidth( minhs.width() - (winSH.width()-desktopRect.width()) );
  }
  return minhs;
}

/* --------------------------------------------------------------------------- */
/* setSizeHint -- used from GuiQtTable                                         */
/* --------------------------------------------------------------------------- */
void GuiQtTableViewBase::setSizeHint (int w, int h) {
#if QT_VERSION > 0x050600
  QRect drect = QGuiApplication::primaryScreen()->availableGeometry();
#else
  QRect drect = QApplication::desktop()->availableGeometry();
#endif
  m_hs.setWidth(w>drect.width() ? drect.width() : w);
  m_hs.setHeight(h>drect.height() ? drect.height() : h);
}

/* --------------------------------------------------------------------------- */
/* setScrollBarHint -- used from GuiQtTable                                    */
/* --------------------------------------------------------------------------- */
void GuiQtTableViewBase::setScrollBarHint (int w, int h)
{
  m_vert_sb_width  = w;
  m_horz_sb_height = h;
}

/* --------------------------------------------------------------------------- */
/* copy --                                                                     */
/* --------------------------------------------------------------------------- */
void GuiQtTableViewBase::copy(bool all) {
  QClipboard *clipboard = QApplication::clipboard();
  long nRows = model()->rowCount();
  long nCols = model()->columnCount();
  std::ostringstream values;
  std::ostringstream hvalues;
  std::ostringstream vvalues;
  std::string sep_char( 1, GuiFactory::Instance()->getDelimiter());
  bool with_headers( false );

  // get all selected Cells
  if (!all) {
    QModelIndexList selIdxs = selectedIndexes();
#if QT_VERSION > 0x050600
    std::sort(selIdxs.begin(), selIdxs.end()); // row sorted
#else
    qSort(selIdxs.begin(), selIdxs.end()); // row sorted
#endif
    if (selIdxs.size()) {
      // only selected
      int old_row=-1;//selIdxs.first().row();
      bool all_selected = false;
      if ( (nRows-verticalHeader()->hiddenSectionCount())*
           (nCols-horizontalHeader()->hiddenSectionCount())  ==selIdxs.size())
        all_selected = true;

      // horz Header if wished
      if (with_headers && horizontalHeader()->isVisible() && all_selected) {
        bool bSep=verticalHeader()->isVisible() ? true : false;
        for (long c = 0; c < nCols; ++c)
          if ( !isColumnHidden(c) && columnWidth(c) > 0) {
            values << (bSep?sep_char:"") << "\""
                   << model()->headerData(c, Qt::Horizontal).toString().trimmed().toStdString()
                   << "\"";
            bSep=true;
          }
        values << std::endl;
      }

      // data
      for (long i = 0; i < selIdxs.size(); ++i) {
        if (isColumnHidden( selIdxs.at(i).column() ) || columnWidth( selIdxs.at(i).column() ) == 0) continue;

        if (old_row != selIdxs.at(i).row()) {
          if (old_row>=0)
            values << std::endl;

          if (with_headers && verticalHeader()->isVisible() && all_selected) { // vertical header
            values  << "\""
                    << model()->headerData(selIdxs.at(i).row(), Qt::Vertical).toString().trimmed().toStdString()
                    << "\"" << sep_char;
          }

          values << selIdxs.at(i).data().toString().trimmed().toStdString();
          old_row=selIdxs.at(i).row();
        } else
#if QT_VERSION >= 0x060000
          if (selIdxs.at(i).data().metaType() == QMetaType(QMetaType::Double)) {
#else
          if (selIdxs.at(i).data().type() == QVariant::Double) {
#endif
            QLocale loc;
            values << (i? sep_char : "") << loc.toString(selIdxs.at(i).data().toDouble()).toStdString();
          } else
            values << (i? sep_char : "") << selIdxs.at(i).data().toString().trimmed().toStdString();
      }
    }
  } else {
    // all data
    getBounds(nRows, nCols);

    // horz Header
    bool bSep = verticalHeader()->isVisible() ? true : false;
    if (horizontalHeader()->isVisible()) {
      for (long c = 0; c < nCols; ++c)
        if ( with_headers &&  !isColumnHidden(c) && columnWidth(c) > 0) {
          values << (bSep?sep_char:"")  << "\""
                 << model()->headerData(c, Qt::Horizontal).toString().trimmed().toStdString()
                 << "\"";
          bSep=true;
        }
      if (with_headers && bSep)
        values << std::endl;
    }
    // data
    if (m_table) {
      for (long r = 0; r < nRows; ++r) {
        long rIdx = r;
        if (m_table->getHLineList().size() &&
            m_table->getHorzLinePlacement() == GuiElement::align_Bottom) {
          rIdx += m_table->getHLineList().size();
          if (rIdx>=nRows)
            rIdx-=nRows;
        }

        if ( isRowHidden(rIdx)) continue;
        bSep = false;
        if (with_headers && verticalHeader()->isVisible()) {
          values  << "\""
                  << model()->headerData(rIdx, Qt::Vertical).toString().trimmed().toStdString()
                  << "\"";
          bSep=true;
        }
        for (long c = 0; c < nCols; ++c) {
          long cIdx=c;
          if (m_table->getVLineList().size() &&
              m_table->getVertLinePlacement() == GuiElement::align_Right) {
            cIdx += m_table->getVLineList().size();
            if (cIdx>=nCols)
              break;
            if (cIdx>=(nCols-m_table->getVLineList().size()))
              cIdx-=(nCols-m_table->getVLineList().size());
          }
          if (isColumnHidden(cIdx) || columnWidth(cIdx) == 0) continue;
          values << (bSep?sep_char:"") << model()->index(rIdx, cIdx).data().toString().trimmed().toStdString();
          bSep=true;
        }
        values << std::endl;
      }
    }
    else {
      for (long r = 0; r < nRows; ++r) {
        for (long c = 1; c < nCols; ++c) {
#if QT_VERSION >= 0x060000
          if (model()->index(r, c).data().metaType() == QMetaType(QMetaType::Double)) {
#else
            if (model()->index(r, c).data().type() == QVariant::Double) {
#endif
            QLocale loc;
            values << (c>1 ? sep_char:"") << loc.toString(model()->index(r, c).data().toDouble()).toStdString();
          } else
            values << (c>1 ? sep_char:"") << model()->index(r, c).data().toString().trimmed().toStdString();
        }
        values << std::endl;
      }
    }
  }
  clipboard->setText( QString::fromStdString(values.str()) );
  clipboard->setText( QString::fromStdString(values.str()), QClipboard::Selection );
}

/* --------------------------------------------------------------------------- */
/* paste --                                                                    */
/* --------------------------------------------------------------------------- */
void GuiQtTableViewBase::paste() {
  if (!m_table) return;
  BUG(BugGui, "GuiQtTableViewBase::paste");

  // einfache Version kompletter Tabelle ueber Stream
  if (0 && m_table) {
    Stream * in_stream = m_table->getStream();
    in_stream->setDelimiter( GuiFactory::Instance()->getDelimiter() );
    setEnabled(false);
    GuiFactory::Instance()->doPaste(in_stream);
    m_table->update(GuiElement::reason_Always);
    setEnabled(true);
    return;
  }

  // hier ist noch etwas zu tun
  char sep_char=GuiQtFactory::Instance()->getDelimiter();
  QString text;
  if (AppData::Instance().HeadlessWebMode()) {
    text = QString::fromStdString(m_table->popArgClipboardText());
  } else {
    QClipboard *clipboard = QApplication::clipboard();
    text = clipboard->text();
    BUG_DEBUG("paste mime["<<clipboard->mimeData()->hasText()
              <<"] owns: Clipboard["<<clipboard->ownsClipboard()
              <<"] findBuffer["<<clipboard->ownsFindBuffer()
              <<"] selection["<<clipboard->ownsSelection()<<"]");
  }
  QStringList slist = text.split(QRegularExpression("\\r{0,1}\\n{1}"));
  if (slist.isEmpty()) return;
  // remove empty last line (LibreOffice Calc adds '\n' to the end)
  if (slist.last().isEmpty()) {
    slist.removeLast();
    if(slist.isEmpty()) return;
  }
  int numCols = slist.at(0).split( sep_char ).size();

  int row = currentIndex().row();
  int col =currentIndex().column();
  QModelIndexList selIdxs = selectedIndexes();
  if (selIdxs.size()) {
    row = selIdxs.first().row();
    col = selIdxs.first().column();
  }
  BUG_DEBUG("paste text["<<text.toStdString()
            <<"] numRows["<<slist.size()
            <<"] numCols["<<numCols<<"]");

  // read data
  GuiElement *elem = m_list!=NULL ? m_list->getElement() : m_table->getElement();
  DataPoolIntens::Instance().BeginDataPoolTransaction( elem->transactionOwner() );
  for (int line=0; line < slist.size(); ++line) {
    // create new row, if needed
    if ((m_table->getTableMatrix() ||
         (!m_table->getTableMatrix() && m_table->getOrientation() == GuiElement::orient_Vertical)) &&
        (line+row) == model()->rowCount()) {
      model()->insertRow(line+row, QModelIndex());
    }
    QStringList sline = slist.at(line).split( sep_char );
    for (int idx=0; idx < sline.size(); ++idx) {
      GuiTableItem *item = m_table->getTableItem( line+row, idx+col );
      // create new column, if needed
      if ((m_table->getTableMatrix() ||
           (!m_table->getTableMatrix() && m_table->getOrientation() == GuiElement::orient_Horizontal)) &&
          (idx+col) == model()->columnCount()) {
        model()->insertColumn(idx+col, QModelIndex());
      }
      if (!item || !item->isEditable() ||
          isColumnHidden(idx+col) || columnWidth(idx+col) == 0) {
        BUG_DEBUG("  no item line["<<line+row<<"] idx["<<idx+col<<"]");
        continue;
      }
      // replace "," to "."
      std::string s(sline.at(idx).toStdString());
      if (AppData::Instance().HeadlessWebMode() &&
          item->getDataField() && item->getDataField()->getDataType() == DataDictionary::type_Real) {
        replaceAll(s, ",", ".");
      }

      // add to temporary  paste object list
      m_pasteDataList.push_back( PasteData( line+row, idx+col, s) );
      BUG_DEBUG(idx << ".Value["<<s<<"]");
    }
  }

  // start timer and start first paste
  if (m_pasteDataList.size()) {
    if (AppData::Instance().HeadlessWebMode()) {
      // im webmode müssen wir hier alle Zellen beschreiben
      // und hoffen, dass die Funktionen später problemlos
      // nach dem Ende dieser Funktion problemlos laufen
      // !!! Updates durch diese Funktionen kommen für diesen
      // Funktionresponse zu spät
      while(m_pasteDataList.size()) {
        const PasteData& pasteData = m_pasteDataList.front();
        BUG_MSG("First Paste Operation Value["<<pasteData.newValue<<"] pos["<<pasteData.row<<","<<pasteData.col<<"] Size["<<m_pasteDataList.size()<<"]");
        GuiTableItem *item = m_table->getTableItem( pasteData.row, pasteData.col );
        GuiQtTableDataItem::setPasteConfirmDialogMode( GuiQtTableDataItem::Activated );
        item->ValidateCell(  pasteData.newValue );
        m_pasteDataList.erase( m_pasteDataList.begin() );

      }
    } else {
      const PasteData& pasteData = m_pasteDataList.front();
      BUG_MSG("First Paste Operation Value["<<pasteData.newValue<<"] pos["<<pasteData.row<<","<<pasteData.col<<"] Size["<<m_pasteDataList.size()<<"]");
      GuiTableItem *item = m_table->getTableItem( pasteData.row, pasteData.col );
      GuiQtTableDataItem::setPasteConfirmDialogMode( GuiQtTableDataItem::Activated );
      item->ValidateCell(  pasteData.newValue );
      m_timer->start();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* clearSelected --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtTableViewBase::clearSelected() {
  if (!m_table) return;
  BUG(BugGui, "GuiQtTableViewBase::clearSelected");

  QModelIndexList idxList = selectedIndexes();
  for (int i = 0; i < idxList.size(); ++i) {
    GuiTableItem *item = m_table->getTableItem(idxList.at(i).row(), idxList.at(i).column());
    if(item == 0) {
      continue;
    }
    item->ValidateCell("");
  }
}

/* --------------------------------------------------------------------------- */
/* clearAll --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtTableViewBase::clearAll() {
  if (!m_table) return;
  BUG(BugGui, "GuiQtTableViewBase::clearAll");
  m_table->clearAllData();
}

/* --------------------------------------------------------------------------- */
/* cancelButtonPressed --                                                      */
/* --------------------------------------------------------------------------- */

void GuiQtTableViewBase::cancelButtonPressed(){
  BUG( BugJobStart, "cancelButtonPressed" );
  m_timer->stop();
  m_pasteDataList.clear();
  GuiQtTableDataItem::setPasteConfirmDialogMode( GuiQtTableDataItem::Inactivated );
  GuiElement *elem = m_list!=NULL ? m_list->getElement() : m_table->getElement();
  DataPoolIntens::Instance().RollbackDataPoolTransaction( elem->transactionOwner() );
  GuiManager::Instance().update(GuiElement::reason_Cancel);
  if (m_dialog) {
    m_dialog->unmanage();
    delete m_dialog;
    m_dialog = 0;
  }
}


/* --------------------------------------------------------------------------- */
/* workPaste --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtTableViewBase::workPaste() {
  BUG(BugGui, "GuiQtTableViewBase::workPaste");
  m_timer->stop();

  // unlovely, install DialogWaitCursor
  // hopefully last function call will remove it
  GuiFactory::Instance()->installDialogsWaitCursor();

  // no dialog active => start next paste operation
  if (GuiDataField::isDialogActive() == true) {
    // other dialog is popup => do unmanage
    if (m_dialog)
      m_dialog->unmanage();
  } else {
    if (m_dialog)
      m_dialog->manage();
    m_pasteDataList.erase( m_pasteDataList.begin() );
    // nothing to do => exit
    if (m_pasteDataList.size() == 0) {
      GuiQtTableDataItem::setPasteConfirmDialogMode( GuiQtTableDataItem::Inactivated );
      GuiElement *elem = m_list!=NULL ? m_list->getElement() : m_table->getElement();
      DataPoolIntens::Instance().CommitDataPoolTransaction( elem->transactionOwner() );
      if (m_dialog) {
        m_dialog->unmanage();
        delete m_dialog;
        m_dialog = 0;
      }
      BUG_MSG("Finished Paste Operation");

      // at the end, start a Empty Dummy function and to get a GuiUpdate
      // and remove DialogsWaitCursor
      JobFunction* func  = JobManager::Instance().getFunction( "PASTE_UPDATEFORMS", true);
      JobManager::Instance().opEndOfFunction( func );
      func->setSilent();
      JobStarter *trigger = new Trigger( this, func);
      trigger->startJob();

      return;
    }

    // start next paste operation
    const PasteData& pasteData = m_pasteDataList.front();
    GuiTableItem *item = m_table->getTableItem( pasteData.row, pasteData.col );
    if (!(item && item->getDataField()))
      return;

    // we do not want GuiUpdates, we do this afterword (see above)
    // temporary clear GuiUpdate flag
    if (item->getDataField()->getFunction()) {
      bool updateForms = item->getDataField()->getFunction()->getUpdateForms();
      item->getDataField()->getFunction()->setUpdateForms( false );
      item->ValidateCell(  pasteData.newValue );
      item->getDataField()->getFunction()->setUpdateForms( updateForms );
    } else
      item->getDataField()->setGuiUpdate(false);
      item->ValidateCell(  pasteData.newValue );
      item->getDataField()->setGuiUpdate(true);
    BUG_MSG("Next Paste Operation Value["<<pasteData.newValue<<"] pos["<<pasteData.row<<","<<pasteData.col<<"]");
  }
  m_timer->start();
}

/* --------------------------------------------------------------------------- */
/* resetSorting --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtTableViewBase::resetSorting() {
  if (m_list)
    m_list->resetSortCriteria();
}

bool GuiQtTableViewBase::edit( const QModelIndex & indexIn, EditTrigger trigger, QEvent * event ) {
  if (m_list) // wir sind in der Liste
    return false;
  QModelIndex index(indexIn);
  GuiTableItem *item = m_table->getTableItem( index.row(), index.column() );

  // handle vertical navigation
  if(!(item && item->getDataField()) ||
     item->getDataField()->Attr()->GuiKind() == UserAttr::gui_kind_combobox){
    m_nextEditable = QModelIndex(); // reset
  } else
    if (m_table->getNavigation() == GuiElement::orient_Vertical && m_nextEditable.isValid() && !isFrozenTablePart(index)) {
    if (event) { // MousePress
      m_nextEditable = QModelIndex();
      index = indexIn;
    } else {
      index = m_nextEditable;
    }
  }

  // button
  if(item && item->getDataField() &&
     item->getDataField()->Attr()->GuiKind() == UserAttr::gui_kind_button){
    if (event && event->type() == QEvent::MouseButtonPress) {
      // imitate push button
      m_pressedButton = index;
      update(index);
      return trigger == NoEditTriggers;
    }

    if (event && event->type() == QEvent::MouseButtonRelease) {
      // color picker
      if (item->getDataField()->Attr()->IsColorPicker()) {
        QColorDialog dlg(m_table->myWidget());
        if (QString::fromStdString(item->CellValue()).trimmed().size())
          dlg.setCurrentColor(QColor(QString::fromStdString(item->CellValue())));
        if (dlg.exec() == QDialog::Accepted) {
          QColor color = dlg.selectedColor();
          item = m_table->getTableItem( index.row(), index.column() ); // muss sein!
          dynamic_cast<GuiQtTableDataItem*>(item)->ValidateCell( color.name().toStdString() );
        }
        return trigger == NoEditTriggers;
      }
      if (QString::fromStdString(item->CellValue()).trimmed().size())
        dynamic_cast<GuiQtTableDataItem*>(item)->ValidateCell( item->CellValue() );
    }
    // reset imitate push button
    m_pressedButton = QModelIndex();
    update(index);
  } else {
    //  general case
    if (index.flags() & Qt::ItemIsEditable && columnWidth(index.column()) != 0) {
      return QTableView::edit(index, trigger, event);
    }
  }

  // not editable, select next
  QModelIndex next;
  if (trigger == QAbstractItemView::CurrentChanged) {
    return true;
  } else {
    clearSelection();
  }
  return trigger != NoEditTriggers;
}

/* --------------------------------------------------------------------------- */
/* getMenu --                                                                  */
/* --------------------------------------------------------------------------- */

GuiPopupMenu* GuiQtTableViewBase::getMenu() {
  if (!m_menu) {
    GuiMenuButton *button = 0;
    GuiElement *elem = m_list!=NULL ? m_list->getElement() : m_table->getElement();

    m_menu = GuiFactory::Instance()->createPopupMenu( elem );
    m_menu->setTitle( _("TableMenu") );
    m_menu->resetMenuPost();
    m_menu->attach( GuiFactory::Instance()->createSeparator(m_menu->getElement())->getElement() );
    if (m_table) {
      m_menu_readonly = GuiFactory::Instance()->createPopupMenu( elem );
      m_menu_readonly->setTitle( _("TableMenu") );
      m_menu_readonly->resetMenuPost();
      m_menu_readonly->attach( GuiFactory::Instance()->createSeparator(m_menu_readonly->getElement())->getElement() );
    }

    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_copyAll );
    button->setLabel( _("Copy All") );
    m_menu->attach( button->getElement() );
    if (m_table) {
      button = GuiFactory::Instance()->createMenuButton( m_menu_readonly->getElement(), &m_copyAll_readonly );
      button->setLabel( _("Copy All") );
      m_menu_readonly->attach( button->getElement() );
    }

    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_copy );
    button->setLabel( _("Copy Selected") );
    m_menu->attach( button->getElement() );
    if (m_table) {
      button = GuiFactory::Instance()->createMenuButton( m_menu_readonly->getElement(), &m_copy_readonly );
      button->setLabel( _("Copy Selected") );
      m_menu_readonly->attach( button->getElement() );
    }

    // list menu: reset sorting
    if (m_list && horizontalHeader()->sortIndicatorSection() > 0) {
      button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_resetSorting );
      button->setLabel( _("Reset Sorting") );
      m_menu->attach( button->getElement() );
    }

    // table menu:
    if (m_table) {
      // paste
      button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_paste );
      button->setLabel( _("Paste") );
      m_menu->attach( button->getElement() );

      // clear selected
      button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_clearSelected );
      button->setLabel( _("Clear Selected") );
      m_menu->attach( button->getElement() );

      // clear all
      button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_clearAll );
      button->setLabel( _("Clear All") );
      m_menu->attach( button->getElement() );
    }
    m_menu->getElement()->create();
    if (m_table) {
      m_menu_readonly->getElement()->create();
    }
  }

  if (m_table && !m_table->isEditable()) {
    return m_menu_readonly;
  }
  return m_menu;
}

/* --------------------------------------------------------------------------- */
/* ResetSortingListener::ButtonPressed --                                      */
/* --------------------------------------------------------------------------- */

void GuiQtTableViewBase::ResetSortingListener::ButtonPressed(){
  // reset sorting, do sort by hidden column 0
  m_table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  m_table->resetSorting();
}

/* --------------------------------------------------------------------------- */
/* ClearListener::ButtonPressed --                                             */
/* --------------------------------------------------------------------------- */

void GuiQtTableViewBase::ClearListener::ButtonPressed(){
  if (m_all) {
   m_table->clearAll();
  } else {
    m_table->clearSelected();
  }
}
