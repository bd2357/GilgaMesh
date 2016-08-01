#include "command.h"

#include <sdk_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <connection.h>
#include "terminal.h"

typedef struct {
  char* name;
  char* description;
  mesh_command_handler callback;
} meshCommand;

static meshCommand** commands;
static uint16_t commandCount;


void print_help(char **parsedCommandArray) {
  UNUSED_PARAMETER(parsedCommandArray);

  for (int i = 0; i < commandCount; i++) {
    meshCommand *nextCommand = commands[i];
    char helpline[100];
    sprintf(helpline, "%-20s%s\r\n", nextCommand->name, nextCommand->description);
    terminal_putstring(helpline);
  }
  terminal_putstring("\r\n");
}

void command_initialize() {
  commands = NULL;
  commandCount = 0;

  mesh_add_terminal_command("help", "Print this help text",     print_help);
//  mesh_add_terminal_command("bro",  "Broadcast a message",      broadcast_logging_message);
  mesh_add_terminal_command("cons", "Print all connections",    print_all_connections);
//  mesh_add_terminal_command("adv",  "Check advertising status", check_advertising_status);
//  mesh_add_terminal_command("scan", "Check scanning status",    check_scanning_status);
//  mesh_add_terminal_command("hb",   "Heartbeat on/off",         toggle_heartbeats);
}

void command_execute(char **parsedCommandArray, uint8_t numberOfItems) {
  UNUSED_PARAMETER(numberOfItems);

  for (int i = 0; i < commandCount; i++) {
    meshCommand *nextCommand = commands[i];
    if (strcmp(parsedCommandArray[0], nextCommand->name) == 0) {
      nextCommand->callback(parsedCommandArray);
      return;
    }
  }
  print_help(parsedCommandArray);
}

void mesh_add_terminal_command(char* commandName, char* commandDescription, mesh_command_handler commandCallback) {
  meshCommand *newCommand = malloc(sizeof(meshCommand));
  newCommand->name = malloc(strlen(commandName) + 1);
  strcpy(newCommand->name, commandName);
  newCommand->description = malloc(strlen(commandDescription) + 1);
  strcpy(newCommand->description, commandDescription);
  newCommand->callback = commandCallback;

  commandCount++;
  commands = realloc(commands, commandCount * sizeof(meshCommand));
  commands[commandCount - 1] = newCommand;
}