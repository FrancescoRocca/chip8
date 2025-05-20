#ifndef EMU_LOG_H
#define EMU_LOG_H

#ifdef EVELOPER
#define EMU_LOG(msg, ...) fprintf(stdout, "[LOG] " msg "\n", ##__VA_ARGS__);
#else
#define EMU_LOG(msg, ...)
#endif

#define EMU_ERROR(msg, ...) fprintf(stderr, "[ERROR] " msg "\n", ##__VA_ARGS__);
#define EMU_INFO(msg, ...) fprintf(stderr, "[INFO] " msg "\n", ##__VA_ARGS__);

#endif
