
#include "DialogUserPassword.h"
#include "gui/GuiFactory.h"
#include <iostream>

/* initialize */
DialogUserPassword* DialogUserPassword::s_instance = 0;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DialogUserPassword::DialogUserPassword(){
}

DialogUserPassword::~DialogUserPassword(){
}


/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

// DialogUserPassword* DialogUserPassword::Instance(UserPasswordListener *listener,
// 						 DialogUnmapListener *unmap){
//   if (s_instance == 0 && listener){
//     s_instance = new QtDialogUserPassword(listener, unmap);
//   }
//   return s_instance;
// }

/*=============================================================================*/
/* Constructor / Destructor of PasswordOkListener                              */
/*=============================================================================*/

DialogUserPassword::PasswordOkListener::PasswordOkListener( DialogUserPassword *d )
  : m_dialog( d ){
}

DialogUserPassword::PasswordOkListener::~PasswordOkListener(){
}

/*=============================================================================*/
/* member functions of PasswordOkListener                                      */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void DialogUserPassword::PasswordOkListener::ButtonPressed(){
  m_dialog->okButtonPressed();
}

/*=============================================================================*/
/* Constructor / Destructor of PasswordOkListener                              */
/*=============================================================================*/

DialogUserPassword::PasswordCancelListener::PasswordCancelListener( DialogUserPassword *d )
  : m_dialog( d ){
}

DialogUserPassword::PasswordCancelListener::~PasswordCancelListener(){
}

/*=============================================================================*/
/* member functions of PasswordCancelListener                                  */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void DialogUserPassword::PasswordCancelListener::ButtonPressed(){
  m_dialog->cancelButtonPressed();
}
