#include "client_reader.h"
#include "stdio.h"
int read_from_client(int client, void *buffer,int byte_to_read, int timeout)
{
	struct timeval start_read_time;
	struct timeval end_read_time;
	time_t time_diff;
	int buffer_offset = 0;
	int byte_read = 0;
	char *char_buffer = (void *)buffer;	
	gettimeofday(&start_read_time, NULL);
	while(byte_to_read != 0)
	{
		gettimeofday(&end_read_time, NULL);
		time_diff = end_read_time.tv_sec - start_read_time.tv_sec ;
		if(time_diff > timeout)
		{
			return buffer_offset;
		}
		char_buffer = char_buffer + buffer_offset;
		byte_read = read(client, char_buffer, byte_to_read);
		if(IS_TRY_AGAIN_READ_ERROR(byte_read))
		{					
			return buffer_offset;
		}
		else if(byte_read < 0)
		{
			return -1;	
		}
		byte_to_read = byte_to_read - byte_read;
		buffer_offset = buffer_offset + byte_read;
	}
	return buffer_offset;
}
