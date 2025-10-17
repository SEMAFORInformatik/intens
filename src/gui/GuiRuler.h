
#if !defined(GUI_RULER_INCLUDED_H)
#define GUI_RULER_INCLUDED_H

/** Ein GuiRuler-Objekt dient einer GuiFieldgroup als Massstab zum Ausrichten
    von horizontal und vertikal angelegten GuiElementen.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiRuler.h,v 1.7 2003/08/29 13:51:08 hed Exp $
*/
class GuiRuler
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiRuler(){
  }
  virtual ~GuiRuler(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setTitleSize( int sz );
  void setSize( int, int );
  void setTitleSpacing( int sz );
  void setSpacing( int, int );
  void setSpacing( int );
  void alignFields( int );
  int TitleSize();
  int Size( int );
  int TitleSpacing();
  int Spacing( int );
  void print();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  class RulerItem
  {
  public:
    RulerItem()
      : m_size( 0 )
      , m_spacing( 0 ){
    }
    int  m_size;
    int  m_spacing;
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::vector<RulerItem> GuiRulerItemList;

  RulerItem          m_title;
  GuiRulerItemList   m_items;

};

#endif
