#ifndef QIN_UTILITY_CRC32_H_
#define QIN_UTILITY_CRC32_H_

//static bool crc_file(const char* file, unsigned long& crc);
unsigned long crc_str(const char* string);
unsigned long crc_buffer(const char* string, size_t size);

#endif
