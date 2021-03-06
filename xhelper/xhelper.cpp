#include "framework.h"
#include "include/xhelper.h"
#ifndef ndo
#include <random>
#endif

long long milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}

XHELPER_API void rInit() {
#ifndef ndo
	std::random_device rd;
	std::uniform_int_distribution<int> dist(0, 20);
#endif
	srand((unsigned)time(0)
#ifndef ndo
		+ dist(rd)
#endif
	);
}
XHELPER_API int random(unsigned int max) {
	return rand() % max;
}


XHELPER_API bool shouldClose(const char* c) {
	//Make sure at most one instance of the tool is running
	HANDLE hMutexOneInstance(::CreateMutexA(NULL, TRUE, c));
	bool bAlreadyRunning((::GetLastError() == ERROR_ALREADY_EXISTS));
	if (hMutexOneInstance == NULL || bAlreadyRunning)
	{
		if (hMutexOneInstance)
		{
			::ReleaseMutex(hMutexOneInstance);
			::CloseHandle(hMutexOneInstance);
		}
		return true;
	}
	return false;
}

void rballangle(int* b)
{
	*b = random(180 - 90) + 115;
}

xhelpers::xhelpers(const int* size)
{
	sizeOfWindow = size;
}

void xhelpers::pongmove(float* ponghigh, bool reason, bool reasonb, float pongspeeds)
{
	if (reason)
		if (*ponghigh - pongspeeds > 60)
			*ponghigh -= pongspeeds;
	if (reasonb)
		if (*ponghigh + pongspeeds < (((float)sizeOfWindow[1]) - 60))
			*ponghigh += pongspeeds;
}

void timer::beginTime()
{
	start = milliseconds_now();
	beginT = true;
}

timer::timer()
{
	beginT = false;
}

double timer::getTimer(double standard) {
	if (beginT) {
		lastV = ((double)(milliseconds_now() - start) / 1000);
		return lastV;
	}
	else
		return standard;
}
