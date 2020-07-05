// httpd_lib.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "httpd.h"

#define MAX (5)

HANDLE	hMutex; //ミューテックスのハンドル

T_HTTP_SOCKET* gThsp = NULL;

T_HTTPD_PORT http_port[] = {
	{HTTPD_TSK_1, 12345},
	{HTTPD_TSK_2, 23456},
	{HTTPD_TSK_3, 34567},
	{HTTPD_TSK_4, 45678},
	{HTTPD_TSK_5, 56789},
};

int		empty()
{
	return gThsp == NULL;
}

int		en_queue(int x)
{
	T_HTTP_SOCKET *next, *p_temp;

	//mutex で  count1 を保護
	printf("ミューテックスの解放を待っています");
	WaitForSingleObject(hMutex, INFINITE);
	next = (T_HTTP_SOCKET *)malloc(sizeof(T_HTTP_SOCKET));
	printf("処理中です...");
	if (next != NULL) { /* メモリが確保できないと NULL が返される */
		WSAStartup(MAKEWORD(2, 0), &next->wsaData);
		next->s_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (next->s_sock == INVALID_SOCKET) {
			printf("socket : %d\n", WSAGetLastError());
			return 1;
		}
		next->sid = x;
		next->server_addr.sin_family = AF_INET;
		for (int i = 0; MAX > i; i++) {
			if (http_port[i].sid == x) {
				next->server_addr.sin_port = htons(http_port[i].port);
			}
		}
		next->server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
		next->yes = 1;
		setsockopt(next->s_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&next->yes, sizeof(next->yes));
		
		/* ローカルアドレスとソケットを関連付けます */
		if (bind(next->s_sock, (struct sockaddr*)&next->server_addr, sizeof(next->server_addr)) != 0) {
			printf("bind : %d\n", WSAGetLastError());
			return 1;
		}
		/* 外部からのコネクションを待機するためにソケットをリスン状態にします */
		if (listen(next->s_sock, 5) != 0) {
			printf("listen : %d\n", WSAGetLastError());
			return 1;
		}
		next->p = NULL;
		p_temp = gThsp;
		/* データが無ければ */
		if (p_temp == NULL) {
			gThsp = next;
		}
		/* データが有れば */
		else {
			while (p_temp->p != NULL) {
				p_temp = p_temp->p;	/* 次ポインタをテンポラリに格納 */
			}
			p_temp->p = next;
		}
		printf("ミューテックスを解放しました");
		ReleaseMutex(hMutex);

		return 1;
	}
	else {
		printf("ミューテックスを解放しました");
		ReleaseMutex(hMutex);
		return 0;
	}

}

int		de_queue(int x)
{
	T_HTTP_SOCKET*	next;

	if (!empty()) {
		WSACleanup();
		next = gThsp->p;
		free(gThsp);
		gThsp = next;
		return 1;
	}
	else {
		/* データがなければ0を返却 */
		return 0;
	}

}

T_HTTP_SOCKET*	take_queue(SOCKETID id)
{
	T_HTTP_SOCKET* next = gThsp;

	//mutex で  count1 を保護
	WaitForSingleObject(hMutex, INFINITE);
	if (!empty()) {
		while (next != NULL) {
			if (next->sid == id) {
				break;
			}
			next = next->p;
		}
	}
	else {
		/* データがなければ0を返却 */
		return NULL;
	}
	ReleaseMutex(hMutex);

	return next;
}

int	init_httpd(int x)
{
	int ret = 0;

	for (int i = 0; i < x; i++) {
		if (en_queue(i+1)) {
			ret += 1;
		}
	}
	return ret;
}

int close_httpd(int x)
{
	int ret = 0;

	for (int i = 0; i < x; i++) {
		if (de_queue(x)) {
			ret += 1;
		}
	}
	return ret;
}

int httpd_main(int x)
{
	T_HTTP_SOCKET* p = NULL;

	char buf[2048];
	char inbuf[2048];

	hMutex = CreateMutex(NULL, FALSE, NULL);	//ミューテックス生成

	if (en_queue(x) <= 0) {
		return 0;
	}

	p = take_queue(x);

	// 応答用HTTPメッセージ作成
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf),
		"HTTP/1.0 200 OK\r\n"
		"Content-Length: 20\r\n"
		"Content-Type: text/html\r\n"
		"\r\n"
		"HELLO WORLD\r\n");

	while (1) {
		p->len = sizeof(p->client_addr);
		p->c_sock = accept(p->s_sock, (struct sockaddr*)&p->client_addr, &p->len);
		if (p->c_sock == INVALID_SOCKET) {
			printf("accept : %d\n", WSAGetLastError());
			break;
		}

		memset(inbuf, 0, sizeof(inbuf));
		recv(p->c_sock, inbuf, sizeof(inbuf), 0);
		// 本来ならばクライアントからの要求内容をパースすべきです
		printf("%s", inbuf);

		// 相手が何を言おうとダミーHTTPメッセージ送信
		send(p->c_sock, buf, (int)strlen(buf), 0);

		closesocket(p->c_sock);
	}

	if (close_httpd(MAX) < 1) {
		return 0;
	}

	CloseHandle(hMutex);

	return 0;
}

