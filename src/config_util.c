#include "config_util.h"
GKeyFile *load_config_file(const char *_config_file_name)
{
	GKeyFile *_config_file=g_key_file_new();
	GKeyFileError *_file_error=NULL;
	if(_config_file==NULL)
	{
		return NULL;
	}
	if(g_key_file_load_from_file(_config_file,_config_file_name,G_KEY_FILE_NONE,&_file_error)==FALSE)
	{
		g_key_file_free(_config_file);
		return NULL;
	}
	return _config_file;	
}
