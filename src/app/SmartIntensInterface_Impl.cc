
#include "streamer/Stream.h"
#include "app/Plugin.h"
#include "app/SmartIntensInterface_Impl.h"
#include "gui/GuiFactory.h"

//-------------------------------------------------
// Construtor / Destructor
//-------------------------------------------------
SmartIntensInterface_Impl::SmartIntensInterface_Impl(Plugin* p) :m_plugin(p) {
}

SmartIntensInterface_Impl::~SmartIntensInterface_Impl() {
}

//-------------------------------------------------
// subscribedData
//-------------------------------------------------
void SmartIntensInterface_Impl::subscribedData(const TransferData& pData) {
  std::vector<Stream*> streams;
  JobFunction* func=0;
  if (m_plugin->getSubscribeData(pData.getHeader(), streams, func)) {

    // read data
    int idx = 0;
    for (std::vector<Stream*>::const_iterator it = streams.begin();
	 it != streams.end(); ++it, ++idx) {
      (*it)->clearRange( );
      if (idx < pData.getData().size()) {
	std::istringstream is( pData.getData()[idx] );
	(*it)->read( is );
      }
    }

    // start function
    if( func ){
      Trigger *trigger = new Trigger( this, func );
      trigger->startJob();
    }
    else GuiFactory::Instance()->update( GuiElement::reason_Process );
  }
}

//-------------------------------------------------
// Trigger::backFromJobStarter
//-------------------------------------------------
void SmartIntensInterface_Impl::Trigger::backFromJobStarter( JobAction::JobResult rslt ) {
}
