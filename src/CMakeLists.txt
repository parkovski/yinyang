set(SOURCES
  main.c
)

add_executable(yinyang ${SOURCES})

if(APPLE)
  target_sources(yinyang PRIVATE listen-macos.m)
  target_link_libraries(yinyang "-framework AppKit")
endif()
