
#if !defined(DIALOG_COPYRIGHT_H)
#define DIALOG_COPYRIGHT_H

class GuiButtonListener;

class DialogCopyright{
public:
  virtual GuiButtonListener * getElement() = 0;
  /*=============================================================================*/
  /* protected Data                                                              */
  /*=============================================================================*/
protected:
  std::string copyright_pixmap;
  std::string copyright_text;
  std::string copyright_text_title;
  std::string copyright_text_additional;
  std::string organization_text;
};

#endif
