# Included by the core's modules/CMakeLists.txt after the `modules` target exists.
#
# The core links the MySQL client library privately into its `database` target,
# so its include directories do not reach module code. PlayerbotsDatabase.cpp
# needs the complete MySQLPreparedStatement type (which pulls in mysql.h) and
# Playerbots.cpp uses ER_BAD_DB_ERROR from mysqld_error.h. Linking the imported
# `mysql` target here propagates those include directories to the module build.
target_link_libraries(modules
  PRIVATE
    mysql)

# Strategy data (YAML rows) is parsed with the fkYAML header from the core's deps/. The module also builds
# against core forks that lack it; there the data layer compiles as a stub that appends no rows.
if(EXISTS "${CMAKE_SOURCE_DIR}/deps/fkYAML/fkYAML/node.hpp")
  target_include_directories(modules
    PRIVATE
      ${CMAKE_SOURCE_DIR}/deps/fkYAML)
  target_compile_definitions(modules
    PRIVATE
      PLAYERBOTS_HAS_FKYAML)
endif()

# Default location of the strategy data, used when AiPlayerbot.StrategyDataPath is empty.
target_compile_definitions(modules
  PRIVATE
    PLAYERBOTS_STRATEGY_DATA_DIR="${CMAKE_CURRENT_LIST_DIR}/data/strategies")
