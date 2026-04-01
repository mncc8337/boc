#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <LittleFS.h>
#include <sensors.h>

extern void write_log_packet(File &file, sensors_data_t &data);

#endif
