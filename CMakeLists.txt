cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_FLAGS "-pthread -Wall -std=c++14 -pedantic ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,undefined -D_GLIBCXX_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_COVERAGE "-g --coverage")
set(LINK_FLAGS "-pthread ${LINK_FLAGS}")

include_directories(.)

add_library(gtest STATIC gtest/gtest-all.cc gtest/gtest_main.cc)

add_executable(std std.cpp)
target_link_libraries(std gtest)

add_executable(khala_vladislav khala_vladislav.cpp khala_vladislav.h)
target_link_libraries(khala_vladislav gtest)

add_executable(berlin_andrei berlin_andrei.cpp berlin_andrei.h)
target_link_libraries(berlin_andrei gtest)

add_executable(yurchenko_artem yurchenko_artem.cpp yurchenko_artem.h)
target_link_libraries(yurchenko_artem gtest)

add_executable(zhdan_egor zhdan_egor.cpp zhdan_egor.h)
target_link_libraries(zhdan_egor gtest)

add_executable(drozdova_aleksandra drozdova_aleksandra.cpp drozdova_aleksandra.h)
target_link_libraries(drozdova_aleksandra gtest)

add_executable(kozelko_sergei kozelko_sergei.cpp kozelko_sergei.h)
target_link_libraries(kozelko_sergei gtest)

add_executable(terentiev_mikhail terentiev_mikhail.cpp terentiev_mikhail.h)
target_link_libraries(terentiev_mikhail gtest)

#add_executable(postnikova_anastasia postnikova_anastasia.cpp postnikova_anastasia.h)
#target_link_libraries(postnikova_anastasia gtest)

add_executable(krivopaltsev_dmitriy krivopaltsev_dmitriy.cpp krivopaltsev_dmitriy.h)
target_link_libraries(krivopaltsev_dmitriy gtest)

add_executable(nisht_konstantin nisht_konstantin.cpp nisht_konstantin.h)
target_link_libraries(nisht_konstantin gtest)

add_executable(bezborodov_pavel bezborodov_pavel.cpp bezborodov_pavel.h)
target_link_libraries(bezborodov_pavel gtest)
