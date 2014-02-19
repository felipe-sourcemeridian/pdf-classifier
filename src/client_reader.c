#include "client_reader.h"
#include "stdio.h"
int read_from_client(int client, void *buffer,int byte_to_read, int timeout)
{
	clock_t begin_read_time = clock();
	clock_t end_read_time = clock();
	int buffer_offset = 0;
	int byte_read = 0;
	char *char_buffer = (void *)buffer;
	double time_spent_read = 0.0f;
	while(byte_to_read != 0)
	{	
		end_read_time = clock();
		time_spent_read = ((double)(end_read_time - begin_read_time)) / CLOCKS_PER_SEC;
		time_spent_read = time_spent_read * 1000.f;
		if((int)time_spent_read > timeout)
		{
			printf("time spent %d\n", (int)time_spent_read);
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
