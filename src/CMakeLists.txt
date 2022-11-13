set(SOURCES
  main.c
)

add_executable(yinyang ${SOURCES})

if(UNIX)
  target_sources(yinyang PRIVATE exec-unix.c)

  if(APPLE)
    target_sources(yinyang PRIVATE listen-macos.m)
    target_link_libraries(yinyang "-framework AppKit")
  endif()
elseif(WIN32)
  #target_sources(yinyang PRIVATE exec-win32.c listen-win32.c)
  message(FATAL_ERROR "Not supported yet.")
endif()

