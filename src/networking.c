#include "networking.h"

#include <winsock2.h> //we have to include winsock2.h before windows.h

#include "includes.h"

char version[512] = "/irebot/ver";
char download[512] = "/irebot/download";
char headers[400] = "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n";
char host_name[400] = "equitatyo.000webhostapp.com";

char* get_request(char* path, int* out_size) {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	int data_len = 4 + strlen(path) + 16 + strlen(host_name) + 28 + 1;
	char* data = malloc(data_len);
	memset(data, 0, data_len);

	sprintf(data, headers, path, host_name);
	data[data_len] = '\0';

	struct hostent* server = gethostbyname(host_name);
	struct sockaddr_in serv_addr;

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	serv_addr.sin_addr.s_addr = *((unsigned long*)server->h_addr);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(80);

	if (connect(s, (const struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		*out_size = -1;
		return NULL;
	}

	char* response = malloc(4096);
	memset(response, 0, 4096);

	send(s, data, strlen(data), 0);

	int response_len = 0;

	int data_left = recv(s, response, 4096, 0);
	response_len += data_left;

	/*while (data_left > 0) { //i do not remember why i did this
		printf(".");
		data_left = recv(s, response + response_len, 4096 - response_len, 0);
		response_len += data_left;
	}*/

	if (out_size) *out_size = response_len;

	closesocket(s);
	WSACleanup();

	return response;
}

char* get_content(char* response) {
	char* content = malloc(8096);
	int content_len;

	int reading_content = false;

	for (int i = 0; i < strlen(response); i++) {
		if (reading_content) {
			content[content_len] = response[i];
			content_len++;
		}

		if (i < 2) continue;
		if (response[i - 2] == '\n' && response[i - 1] == '\r' && response[i] == '\n') reading_content = true;
	}

	content[content_len] = '\0';

	return content;
}

char* get_version() {
#ifdef __DEBUG
	return 0;
#endif

	int response_len;
	char* response = get_request(version, NULL);
	return get_content(response);
}

void update_to_latest() {
#ifdef __DEBUG
	return;
#endif
	
	char* response = get_request(download, NULL);

	char* url = get_content(response);

	URLDownloadToFile(NULL, url, "temp.exe", 0, NULL);

	char data[] = "@echo off\ntaskkill /F /IM irebot.exe\ntimeout /T 2\ndel irebot.exe\ncopy irebot.exe irebot.exe.bak\nmove temp.exe irebot.exe\nstart irebot.exe";
	FILE* batch_h = fopen("update.bat", "w");
	fwrite(data, strlen(data), 1, batch_h);
	fclose(batch_h);
	system("start cmd /c \"update.bat\"");
}