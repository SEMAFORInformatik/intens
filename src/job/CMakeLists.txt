
add_library(job OBJECT
        InitialWorker.cc
        JobAction.cc
        JobActionSimple.cc
        JobClientSocket.cc
        JobCodeAttrEditableCycle.cc
        JobCodeAttributes.cc
        JobCodeClearTextWindow.cc
        JobCodeCycle.cc
        JobCodeEvalAssign.cc
        JobCodeExecute.cc
        JobCodeFile.cc
        JobCodeMessages.cc
        JobCodeOperator.cc
        JobCodeOthers.cc
        JobCodePush.cc
        JobCodeRestService.cc
        JobCodeStandards.cc
        JobCodeTimer.cc
        JobConfirmation.cc
        JobController.cc
        JobDataReference.cc
        JobEngine.cc
        JobFileDialog.cc
        JobFunction.cc
        JobHardcopy.cc
        JobIncludes.cc
        JobManager.cc
        JobMessage.cc
        JobMsgQueuePublish.cc
        JobRequest.cc
        JobStack.cc
        JobStackData.cc
        JobStackDataComplex.cc
        JobStackDataEndOfLine.cc
        JobStackDataInput.cc
        JobStackDataInteger.cc
        JobStackDataReal.cc
        JobStackDataString.cc
        JobStackDataStructure.cc
        JobStackDataVariable.cc
        JobStackItem.cc
        JobStarter.cc
        JobSubscribe.cc
        JobTask.cc
        JobVarDataReference.cc
        JobWebApiResponse.cc
        )

if (Protobuf_FOUND)
add_dependencies(job intens_protobuf)
endif (Protobuf_FOUND)
target_include_directories(job
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src)

target_link_libraries(job
  PRIVATE
  LibXml2::LibXml2
  Qt::Core
  Qt::Widgets
  Qt::Gui)
