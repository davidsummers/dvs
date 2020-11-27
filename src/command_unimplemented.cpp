#include "command_unimplemented.h"

Error UnimplementedCommand::ParseArgs( DocOptArgs & )
{
  return "Command not yet implemented.";
}

Error UnimplementedCommand::operator ( )( DVS & )
{
  return "Command not yet implemented.";
}
