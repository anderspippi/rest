add_library(uiiitrest STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/datatype.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/problemdetails.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
)

target_link_libraries(uiiitrest
  uiiitsupport
  ${GLOG}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPP_REST_LIBRARY}
)
