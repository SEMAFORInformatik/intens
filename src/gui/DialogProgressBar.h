
#if !defined(DIALOGPROGRESSBAR_H)
#define DIALOGPROGRESSBAR_H

#include <string>
#include "operator/InputChannelEvent.h"
#include "streamer/BasicStream.h"

class Stream;
class JobController;

class DialogProgressBar
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  DialogProgressBar(const std::string& name);
  virtual ~DialogProgressBar();

  /** get GuiElement
   @return GuiElement Pointer
  */
  virtual GuiElement* getGuiElement() = 0;
  /** set JobContoller
  */
  virtual void setJobController(JobController *controller) {
    m_controller = controller;
  }

  /** get WindowTitle
   @return window title
  */
  std::string getWindowTitle();
  /** get LoopTitle
   @return looptitle
  */
  std::string getLoopTitle();
  /** get Main Title
   @return main title
  */
  std::string getMainTitle();
  /** get Main Percent, only integer in range [0, 100] are allowed
   @return main percent rate
  */
  int getMainPercentRate();
  /** get Main Footer
   @return main footer
  */
  std::string getMainFooter();
  void setMainFooter(const std::string& s);

  /** get SubTitle
   @return subtitle
  */
  std::string getSubTitle();
  /** get SubPercent, only integer in range [0, 100] are allowed
   @return sub percent rate
  */
  int getSubPercentRate();

  /** get Data Tile
   @return data title
  */
  std::string getDataTitle();
  /** get Data Footer
   @return data footer
  */
  std::string getDataFooter();
  /** get Data Label Vector
   @return data label vector
  */
  void getDataLabelVector(std::vector<std::string>& vec);
  /** get Data
   @return data
  */

  void getDataVector(std::vector<std::string>& vec);
  /** get Error
   @return error
  */
  std::string getError();

  /** get Abort Command
   @return abort command
  */
  std::string getAbortCommand();

  /** get Pixmap
   @return DataReference of pixmap
  */
  DataReference* getPixmapDataReference();

  /** get Detail Pixmap
   @return DataReference of pixmap
  */
  DataReference* getDetailPixmapDataReference();

  /** execute Abort Command
      @param callAbortedFunc if false, do not call AFTER_PROGRESS_DIALOG_ABORTED Function
  */
  virtual void execute_abort(bool callAbortedFunc=true) = 0;

  /** create a progress fieldgroup
      @param fg bare gui fielfgroup
  */
  void create_fieldgroup(GuiFieldgroup* fg);

  static const std::string DIALOG_NAME;
  static const std::string FIELDGROUP_NAME;

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
 protected:
  bool isDataItemUpdated(TransactionNumber trans);
  void abortJobController();
  void resetData();

  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdate);
#endif

 private:
  JobController* m_controller;
};

#endif
