# CMakeLists_src_dir
#------------------------------------------
# Let's start by defining the include directory.
include_directories(${TISERCONE_SOURCE_DIR}/include)

#Give all the source files one global name
set(tisercone_SOURCES main.c eth_sock.c io_process.c simple_client.c io_interface.c  log_writer.c  msg_handler.c)


