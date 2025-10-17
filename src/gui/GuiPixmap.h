
#if !defined(GUI_PIXMAP_INCLUDED_H)
#define GUI_PIXMAP_INCLUDED_H
#if HAVE_PROTOBUF
#include "protobuf/Message.pb.h"
#endif

class GuiElement;

class GuiPixmap
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiPixmap();
  virtual ~GuiPixmap();

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
/*   virtual void manage() = 0; */
/*   virtual bool destroy() = 0; */
/*   void setSize( int w, int h ) {m_width = w; m_height = h;} */
  virtual void setSize( int w, int h ) = 0;
/*   virtual void getSize( int &w, int &h ) = 0; */

  virtual GuiElement *getElement() = 0;
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false) = 0;
#endif

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
/* public: */
/*   virtual void confirmOkButtonPressed() = 0; */
/*   virtual void confirmCancelButtonPressed() = 0; */

 protected:
  bool isTypeSvg(const std::string& data);
 private:
  GuiPixmap( const GuiPixmap &pixmap );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
/* private: */
/*   int                 m_width; */
/*   int                 m_height; */

};

#endif
