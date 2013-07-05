#include "crc32.h"

class crc_result
{
public:
	crc_result(){
		m_sum = 0;
	}
	int m_sum;
	void process_byte(char c){
		m_sum += m_sum * 5 + c;
	}
	unsigned long  checksum() const{
		return m_sum;
	}
};

unsigned long crc_str(const char* string)
{
	crc_result result;
	while(*string != '\0')
	{
		result.process_byte(*string);
		string++;
	}
	return result.checksum();
}

unsigned long crc_buffer(const char* string, size_t size)
{
	crc_result result;
	while(size--)
	{
		result.process_byte(*string);
		string++;
	}
	return result.checksum();
}

