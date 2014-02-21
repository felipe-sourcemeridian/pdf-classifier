#include "client_writer.h"
int write_to_client(int client, void *data, int bytes_to_write)
{
	int bytes_written = 0;
	bytes_written = write(client, data, bytes_to_write);
	if(bytes_written != -1)
	{
		return bytes_written;
	}
	if(!IS_TRY_AGAIN_WRITE_ERROR(bytes_written))
	{
		return -1;
	}
	return 0;
}
