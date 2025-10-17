
add_library(operator OBJECT
        BatchProcess.cc
        Channel.cc
        ClientSocket.cc
        DaemonProcess.cc
        FileStream.cc
        InputChannelEvent.cc
        IntensServerSocket.cc
        MFMServerSocket.cc
        MatlabInitProcess.cc
        MatlabProcess.cc
        MessageQueue.cc
        MessageQueuePublisher.cc
        MessageQueueReply.cc
        MessageQueueRequest.cc
        MessageQueueSubscriber.cc
        MessageQueueThreads.cc
        OutputChannelEvent.cc
        Process.cc
        ProcessGroup.cc
        ProcessWrapper.cc
        ReportStream.cc
        ReportStreamFileBuffer.cc
        RequestInterface.cc
        RestService.cc
        ServerSocket.cc
        SubscribeInterface.cc
        TimerFunction.cc
        Worker.cc
        ### HAVE_QT
        QtClientSocket.cc
        QtServerSocket.cc
        qostream.cc
        )

target_include_directories(operator
        PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/third-party/jwt-cpp/include
        ${CMAKE_BINARY_DIR}/src )

target_link_libraries(operator
   PRIVATE
   LibXml2::LibXml2
   Qt::Core
   Qt::Widgets
   Qt::Network)
