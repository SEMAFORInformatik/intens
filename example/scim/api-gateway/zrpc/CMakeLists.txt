IF(NOT WIN32)
set(srcs
  apilog.py app.py __init__.py testing.py defroutes.py)

install(FILES ${srcs} DESTINATION api-gateway/zrpc)
ENDIF(NOT WIN32)
