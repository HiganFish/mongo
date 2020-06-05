SET(HEADFILES
        HttpServer.h
        HttpContext.h
        HttpRequest.h
        HttpResponse.h
        HttpRouting.h
        )

SET(SRCFILES
        HttpServer.cpp
        HttpContext.cpp
        HttpRequest.cpp
        HttpResponse.cpp
        HttpRouting.cpp
        )

add_library(mongo_http ${HEADFILES} ${SRCFILES})
target_link_libraries(mongo_http mongo_net)