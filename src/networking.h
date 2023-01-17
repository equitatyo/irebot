#ifndef _NETWORKING_H_
#define _NETWORKING_H_


char* get_request(char* path, int* out_size);
char* get_content(char* full_response);
char* get_version();
void update_to_latest();

#endif //_NETWORKING_H_