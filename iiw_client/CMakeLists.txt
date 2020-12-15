cmake_minimum_required(VERSION 3.17)
project(iiw_client C)

set(CMAKE_C_STANDARD 11)

add_executable(iiw_client main.c client_commands/help_client.h client_commands/list_client.h client_commands/get_client.h client_commands/put_client.h utilities/utilities.h client_commands/settings_client.h utilities/client_upload.h utilities/client_download.h utilities/send_command.h)