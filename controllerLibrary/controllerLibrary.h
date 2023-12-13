#ifdef CONTROLLERLIBRARY_EXPORTS
#define CONTROLLERLIBRARY_API __declspec(dllexport)
#else
#define CONTROLLERLIBRARY_API __declspec(dllimport)
#endif

struct COORDS {
public:
	float x, y;
};
struct Gamepad {
public:
	unsigned short buttons; /* Based on the bitmask of wButtons https://learn.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad */
	unsigned char leftTrigger, rightTrigger;
	float leftMagnitude, rightMagnitude;
	COORDS nL, nR;
};

class CONTROLLERLIBRARY_API CcontrollerLibrary {
private:
	void* pr;
	unsigned long controllerID = 0;
	Gamepad gp;

	bool cached = false;
	unsigned long vibCacheL = 0, vibCacheR = 0;
public:
	CcontrollerLibrary(unsigned long controllerID = 0);
	bool isReady();
	const Gamepad* update();
	void vibrateActiveController(unsigned long L = 0, unsigned long R = 0, bool c = true);
	~CcontrollerLibrary();
};
