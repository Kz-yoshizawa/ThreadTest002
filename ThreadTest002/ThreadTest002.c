//マルチスレッドプログラムと mutex の使い方
#include "httpd.h"

int main(void);

int count1 = 0; //mutex で変数を保護する
int count2 = 0; //保護しない

void func1(LPVOID);
void func2(LPVOID);
void func3(LPVOID);
//void func4(LPVOID, int);
//void func5(LPVOID, int);


void func1(LPVOID pParam)
{
	httpd_main(HTTPD_TSK_1);
}

void func2(LPVOID pParam)
{
	//while (1) {
	//	//mutex で  count1 を保護
	//	WaitForSingleObject(hMutex, INFINITE);
	//	count1 = 0;
	//	ReleaseMutex(hMutex);

	//	count2 = 0;
	//}

	httpd_main(HTTPD_TSK_2);
}

void func3(LPVOID pParam)
{
	httpd_main(HTTPD_TSK_3);
}

int main(void) {
	HANDLE	hThread[3];

	//hMutex = CreateMutex(NULL, FALSE, NULL);	//ミューテックス生成

	hThread[0] = (HANDLE)_beginthread(func1, 0, NULL);	//スレッド1作成
	hThread[1] = (HANDLE)_beginthread(func2, 0, NULL);	//スレッド2作成
	hThread[2] = (HANDLE)_beginthread(func3, 0, NULL);	//スレッド3作成

	//スレッド１、２終了待ち
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	//ハンドルクローズ
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);
	//CloseHandle(hMutex);

	return 0;
}