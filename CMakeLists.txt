CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
PROJECT(unqlite)
INCLUDE(GNUInstallDirs)

# If the user did not customize the install prefix,
# set it to live under build so we don't inadvertently pollute /usr/local
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

OPTION(ENABLE_THREADS "Enable threads (C-API ONLY)" OFF)

IF(${ENABLE_THREADS})
  ADD_DEFINITIONS("-DUNQLITE_ENABLE_THREADS")
ENDIF(${ENABLE_THREADS})

INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/include
)

SET(HEADERS_UNQLITE
        ${CMAKE_CURRENT_LIST_DIR}/include/unqlite.h
        ${CMAKE_CURRENT_LIST_DIR}/include/jx9.h
        ${CMAKE_CURRENT_LIST_DIR}/include/jx9Int.h
        ${CMAKE_CURRENT_LIST_DIR}/include/unqliteInt.h
)

SET(SOURCES_UNQLITE

        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_api.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_builtin.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_compile.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_const.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_hashmap.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_json.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_lex.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_lib.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_memobj.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_parse.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_vfs.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/jx9/jx9_vm.c

        ${CMAKE_CURRENT_LIST_DIR}/src/core/os/os.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/os/os_unix.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/os/os_win.c

        ${CMAKE_CURRENT_LIST_DIR}/src/core/unqlite/api.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/unqlite/bitvec.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/unqlite/fastjson.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/unqlite/lhash_kv.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/unqlite/mem_kv.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/unqlite/pager.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/unqlite/unqlite_jx9.c
        ${CMAKE_CURRENT_LIST_DIR}/src/core/unqlite/unqlite_vm.c
)

SET(UNQLITE_STATIC_LIB unqlite)
ADD_LIBRARY(${UNQLITE_STATIC_LIB} STATIC ${HEADERS_UNQLITE} ${SOURCES_UNQLITE})

INSTALL(TARGETS ${UNQLITE_STATIC_LIB} COMPONENT devel ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
INSTALL(FILES ${HEADERS_UNQLITE} COMPONENT devel DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# Build examples
ADD_EXECUTABLE(Example_1 ${CMAKE_CURRENT_LIST_DIR}/example/1.c ${HEADERS_UNQLITE} ${SOURCES_UNQLITE})
ADD_EXECUTABLE(Example_2 ${CMAKE_CURRENT_LIST_DIR}/example/2.c ${HEADERS_UNQLITE} ${SOURCES_UNQLITE})
ADD_EXECUTABLE(Example_3 ${CMAKE_CURRENT_LIST_DIR}/example/3.c ${HEADERS_UNQLITE} ${SOURCES_UNQLITE})
ADD_EXECUTABLE(Example_4 ${CMAKE_CURRENT_LIST_DIR}/example/4.c ${HEADERS_UNQLITE} ${SOURCES_UNQLITE})
ADD_EXECUTABLE(Example_5 ${CMAKE_CURRENT_LIST_DIR}/example/5.c ${HEADERS_UNQLITE} ${SOURCES_UNQLITE})
ADD_EXECUTABLE(Example_6 ${CMAKE_CURRENT_LIST_DIR}/example/6.c ${HEADERS_UNQLITE} ${SOURCES_UNQLITE})
ADD_EXECUTABLE(Example_huge ${CMAKE_CURRENT_LIST_DIR}/example/unqlite_huge.c ${HEADERS_UNQLITE} ${SOURCES_UNQLITE})
ADD_EXECUTABLE(Example_mp3 ${CMAKE_CURRENT_LIST_DIR}/example/unqlite_mp3.c ${HEADERS_UNQLITE} ${SOURCES_UNQLITE})
ADD_EXECUTABLE(Example_tar ${CMAKE_CURRENT_LIST_DIR}/example/unqlite_tar.c ${HEADERS_UNQLITE} ${SOURCES_UNQLITE})