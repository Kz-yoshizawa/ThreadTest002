// pch.h: プリコンパイル済みヘッダー ファイルです。
// 次のファイルは、その後のビルドのビルド パフォーマンスを向上させるため 1 回だけコンパイルされます。
// コード補完や多くのコード参照機能などの IntelliSense パフォーマンスにも影響します。
// ただし、ここに一覧表示されているファイルは、ビルド間でいずれかが更新されると、すべてが再コンパイルされます。
// 頻繁に更新するファイルをここに追加しないでください。追加すると、パフォーマンス上の利点がなくなります。


#ifndef _HTTPD_H_
#define _HTTPD_H_

#include <stdio.h>
#include <stdlib.h>
// Winsock
#include <winsock2.h>
#include <windows.h>
#include <process.h>

typedef int SOCKETID;

typedef int(*p_httpd_func_type)(void);

typedef enum
{
	HTTPD_TSK_1 = 1,
	HTTPD_TSK_2,
	HTTPD_TSK_3,
	HTTPD_TSK_4,
	HTTPD_TSK_5
} SID_NUM;

typedef struct t_http_socket {

	WSADATA					wsaData;
	SOCKET					s_sock;
	struct sockaddr_in		client_addr;
	int						len;
	SOCKET					c_sock;
	struct sockaddr_in		server_addr;
	SOCKETID				sid;
	BOOL					yes;
	struct t_http_socket*	p;

} T_HTTP_SOCKET;

typedef struct t_httpd_port {

	SOCKETID sid;
	unsigned int port;

} T_HTTPD_PORT;

int		en_queue(int);
int		de_queue(int);
T_HTTP_SOCKET*	take_queue(SOCKETID);
int		init_httpd(int);
int		close_httpd(int);
int		httpd_main(int);


#endif //_HTTPD_H_
