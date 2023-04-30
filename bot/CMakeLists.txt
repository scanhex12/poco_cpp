if (TEST_SOLUTION)
  include_directories(../tests/bot)
  file(GLOB SOLUTION_SRC CONFIGURE_DEPENDS "../tests/bot/telegram/*.cpp")
  set(SOLUTION_TEST_SRC ../tests/bot/test/test_api.cpp)
else()
  file(GLOB SOLUTION_SRC CONFIGURE_DEPENDS "telegram/*.cpp")
  set(SOLUTION_TEST_SRC test/test_api.cpp)
endif()

if (APPLE)
  link_directories(/usr/local/lib)
  link_directories(/usr/local/opt/openssl/lib)

  include_directories(/usr/local/include)
  include_directories(/usr/local/opt/openssl/include)
endif()

add_library(telegram ${SOLUTION_SRC})
target_link_libraries(telegram PocoNet PocoNetSSL PocoFoundation PocoJSON PocoUtil)

if (TEST_SOLUTION)
  add_executable(bot-run ../tests/bot/telegram/main.cpp)
else()
  add_executable(bot-run telegram/main.cpp)
endif()
target_link_libraries(bot-run telegram)

add_executable(fake fake/fake_main.cpp)
target_link_libraries(fake telegram)

add_catch(test_telegram ${SOLUTION_TEST_SRC} fake/fake.cpp fake/fake_data.cpp)
target_link_libraries(test_telegram telegram)
