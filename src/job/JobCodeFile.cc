
#include "job/JobCodeFile.h"
#include "job/JobFileDialog.h"
#include "job/JobStackData.h"
#include "job/JobDataReference.h"

#include "operator/FileStream.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* setDirname --                                                               */
/* --------------------------------------------------------------------------- */

void JobCodeFileDialog::setDirname( const std::string &dirname ){
  m_dirname = dirname;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeFileDialog::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeFileDialog::execute");

  JobFileDialog *filedialog = new JobFileDialog( eng );

  filedialog->setElement( eng->getFunction()->getGuiElement() );
  if( m_dirname.empty() ){
    if( m_dirnamexfer ){
      filedialog->setDirnameXfer( m_dirnamexfer );
    }
  }
  else {
    filedialog->setDirname( m_dirname );
  }
  filedialog->setFilter( m_filter );
  filedialog->showDirOnly( m_dironly );
  filedialog->openMode( m_open );
  filedialog->setDialog( this );

  eng->getFunction()->nextJobAction( filedialog );
  return op_Wait;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpenFile::execute( JobEngine *eng ){
   BUG_DEBUG("JobCodeOpenFile::execute");

   JobAction* jobaction = 0;
   std::string s;

   // FileStream
   if( m_fileStream ){
     if( !m_filename.empty() ){
       jobaction = m_fileStream->getOpenAction( m_filename );
     }
     else if( m_readStack ){
       JobStackDataPtr f( eng->pop() );
       f->getStringValue( s );
       jobaction = m_fileStream->getOpenAction(s);
       // get filename
       m_fileStream->setDataFilename(f->reference().getDataReference());
     }
     else{
       jobaction = m_fileStream->getOpenAction( 0 );
     }
     BUG_DEBUG("JobCodeOpenFile::execute fileName["<<s<<"]");
   }

   if( jobaction ){
     eng->getFunction()->nextJobAction( jobaction );
     return op_Wait;
   }

   std::cerr << "Open Do not know, what to do! fileStream["<<m_fileStream->Name()<<"] readStack["<<m_readStack<<"] fileName["<<m_filename<<"]\n";
   return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSaveFile::execute( JobEngine *eng ){
   BUG_DEBUG("JobCodeSaveFile::execute");

   JobAction* jobaction = 0;
   std::string s;

   // GuiElement
   if( m_guiElement ){
     if( !m_filename.empty() ){
       jobaction = m_guiElement->getSaveAction( m_filename );
     }
     else if( m_readStack ){
       JobStackDataPtr f( eng->pop() );
       f->getStringValue( s );
       jobaction = m_guiElement->getSaveAction(s);
     }
     else{
       jobaction = m_guiElement->getSaveAction( 0 );
     }
   }
   // FileStream
   else if( m_fileStream ){
     if (!m_filename.empty()) {
       jobaction = m_fileStream->getSaveAction( m_filename );
     }
     else if( m_readStack ){
       JobStackDataPtr f( eng->pop() );
       f->getStringValue( s );
       jobaction = m_fileStream->getSaveAction(s);
     }
     else if( m_readBaseStack ){
       JobStackDataPtr f( eng->pop() );
       f->getStringValue( s );
       jobaction = m_fileStream->getSaveAction("", s);
     }
     else{
       jobaction = m_fileStream->getSaveAction( 0 );
     }
   }

   if( jobaction ){
     eng->getFunction()->nextJobAction( jobaction );
     return op_Wait;
   }

   std::cerr << "Save Do not know, what to do! fileStream["<<m_fileStream<<"] guiElement["<<m_guiElement<<"] readStack["<<m_readStack<<"] fileName["<<m_filename<<"]\n";
   return op_Ok;
}
