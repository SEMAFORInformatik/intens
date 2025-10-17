
#if !defined(QT_DIALOG_INFORMATION_H)
#define QT_DIALOG_INFORMATION_H

#include "gui/InformationListener.h"
#include "gui/GuiElement.h"

class QtDialogInformation
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  QtDialogInformation( InformationListener *listener );
  virtual ~QtDialogInformation() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static bool showDialog( GuiElement *, const std::string &, const std::string &
                        , InformationListener * );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void setTitle( const std::string &title ) { m_title = title; }
  void setMessage( const std::string &msg ) { m_msg = msg; }
  bool isMultiFont();
  bool setLabelList();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  std::string             m_title;
  std::string             m_msg;
};

#endif
