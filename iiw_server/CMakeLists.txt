cmake_minimum_required(VERSION 3.17)
project(iiw_server)

set(CMAKE_C_STANDARD 11)

add_executable(iiw_server main.c server_commands/get_server.h server_commands/put_server.h server_commands/list_server.h utilities/utilities.h utilities/server_download.h utilities/server_upload.h utilities/server_command_ack.h)