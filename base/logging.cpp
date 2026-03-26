// Stub logging implementation for macOS
#include "logging.h"
#include <sstream>

namespace base
{

int GetMinLogLevel() {
    return 0;
}

// Stub constructors to satisfy linking
LogMessage::LogMessage(const char* file, int line) : severity_(LOG_INFO), message_start_(0) {}
LogMessage::LogMessage(const char* file, int line, LogSeverity severity) : severity_(severity), message_start_(0) {}
LogMessage::~LogMessage() {}

LogMessage::SaveLastError::SaveLastError() {}
LogMessage::SaveLastError::~SaveLastError() {}

} // namespace base
