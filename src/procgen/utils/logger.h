#ifndef _PROCGEN_UTILS_H
#define _PROCGEN_UTILS_H
#include <cstdarg>
#include <cstdio>

namespace ProcGen {
#define STRIZE(x) #x
#define STR(x) STRIZE(x)

#ifndef NDEBUG
#define LOG_DEBUG(...) ProcGen::Logger::GetLog().Log("Debug", __VA_ARGS__)
#else
#define LOG_DEBUG(...) ;
#endif

#define LOG_ERROR(...) ProcGen::Logger::GetLog().Log("Error", __VA_ARGS__)
#define LOG_INFO(...) ProcGen::Logger::GetLog().Log("Info", __VA_ARGS__)

    class Logger {
      private:
        FILE *output;

      public:
        Logger() : output(stderr) {}
        static Logger GetLog() {
            static Logger log;
            return log;
        }
        void Log(const char *place, const char *message, ...) {
            va_list args;
            va_start(args, message);
            fprintf(output, "[%s] ", place);
            vfprintf(output, message, args);
            va_end(args);
        }
    };
}
#endif

