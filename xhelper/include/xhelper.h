#ifdef XHELPER_EXPORTS
#define XHELPER_API __declspec(dllexport)
#else
#define XHELPER_API __declspec(dllimport)
#endif

XHELPER_API int random(unsigned int max);
XHELPER_API void rInit();
XHELPER_API bool shouldClose(const char* guid);
class XHELPER_API xhelpers {
private:
	const int* sizeOfWindow;
public:
	xhelpers(const int* size);
	void pongmove(float* ponghigh, bool reason = false, bool reasonb = false, float pongspeeds = 6.f);
};

class XHELPER_API timer {
private:
	long long start;
	bool beginT;
	double lastV;
public:
	void beginTime();
	timer();
	double getTimer(double standard = .1);
	const double getLast() { return lastV; }
};
XHELPER_API float cosminusone(float v);
XHELPER_API float cosn(float v);
XHELPER_API float sinn(float v);
XHELPER_API void rballangle(int* b);
XHELPER_API float roundn(float i);
XHELPER_API float tanminusone(float v);

XHELPER_API const char* rndst(int r);
XHELPER_API const char* getDocPath();
