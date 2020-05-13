SET(HEADFILES
        HttpServer.h
        HttpContext.h
        HttpRequest.h
        HttpResponse.h
        )

SET(SRCFILES
        HttpServer.cpp
        HttpContext.cpp
        HttpRequest.cpp
        HttpResponse.cpp
        )

add_library(mongo_http ${HEADFILES} ${SRCFILES})
target_link_libraries(mongo_http mongo_net)