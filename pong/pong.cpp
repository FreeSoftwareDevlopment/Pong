#define useAudio
// UNCOMMENT THE NEXT LINE TO DISABLE AUDIO FOR YOUR COMPILED VERSION
// #undef useAudio
#include "includes.h"

#define pongspeed 400
#define ballspeed 421
#define appguid "{03878e53-d462-4c67-9b36-ec7192f49a8b}" /* POWERED BY https://www.guidgen.com/ */

using namespace sf;

const char gm[] = "Pong";

void renderThreadp(
	RenderWindow* window,
	std::vector<Drawable*>* d,
	std::atomic<double>* myfps,
	const int* size,
	bool* record,
	Shape* dn);

std::string createHelpText(bool muted, int* score, bool record = false) {
	return std::string("Keys:\n") +
		std::string((!record) ? "Record" : "Stop Record") +
		std::string(": R  \n") +
#ifdef useAudio
		std::string(muted ? "Music" : "Mute") +
		std::string(": M  \n") +
#endif
		std::string("\nPlayer 1:\n\tUp: W\n\tDown: S\n\nPlayer 2:\n\tUp: O\n\tDown: L\n\nExit: ESC\n\nScore:\n\tP1: ")
		+ std::to_string(score[0]) +
		std::string("\n\tP2: ") + std::to_string(score[1]);
}

bool detectCollision(const Vector2f& centerOfCircle, const Vector2f& posOfObj, bool p1) {
	return centerOfCircle.y > posOfObj.y && centerOfCircle.y < posOfObj.y + 120 &&
		(p1 ? (posOfObj.x + 10 > centerOfCircle.x) : (posOfObj.x < centerOfCircle.x)
			&& ((!p1) ? (posOfObj.x + 10 < centerOfCircle.x) : (posOfObj.x > centerOfCircle.x)));
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	const int size[]{ 960, 640 };/* HEIGHT, WIDTH */
#define widthx (float)size[0]
#define heightx (float)size[1]

	xhelpers xhelp(&size[0]);
	std::vector<Drawable*> d;

	bool ui{ true };

	RenderWindow window(VideoMode(size[0], size[1]), L"Pong - \uAC8C\uC784");
	HWND windowhandle = window.getSystemHandle();

	/* RANDOM INIT */
	rInit();

	if (shouldClose(appguid)) {
		MessageBoxA(windowhandle,
			"OOPS, ONE INSTANCE OF \"Pong\" already running,\nClose this Version before start a new",
			gm, MB_OK | MB_ICONHAND | MB_TOPMOST);
		return 1;
	}
	int score[2]{ 0,0 };

	CircleShape circle(10);
	{
		circle.setFillColor(Color(0xff, 0xff, 0x0));
		circle.setOrigin(25, 25);
		circle.setOutlineThickness(.4f);
		circle.setOutlineColor(Color::White);
		d.push_back(&circle);
		circle.setScale(Vector2f(.4f, .4f));
	}
	CircleShape c3(20);
	{
		c3.setOrigin(25, 25);
		c3.setPosition(Vector2f(widthx, heightx));
	}

	sf::Font RobotoBlack;
	ui = RobotoBlack.loadFromFile("fonts/Roboto/Roboto-Black.ttf");

	sf::Text text, tbmute;

	if (ui) {
		text.setFont(RobotoBlack);
		text.setRotation(90);
		text.setPosition((float)widthx, 0);
		d.push_back(&text);

		tbmute.setFont(RobotoBlack);
		tbmute.setPosition(0, 0);
		tbmute.setCharacterSize(10);
		tbmute.setString("Loading...");
		tbmute.setFillColor(Color::Red);
		d.push_back(&tbmute);

		text.setCharacterSize(10);

		text.setFillColor(Color::Blue);
		text.setString("PONG");
	}

	sf::Vector2f rectSize = sf::Vector2f(10.f, 120.f);
	sf::RectangleShape p1(rectSize), p2(rectSize);
	float pongHigth[] = { (heightx * 0.5), (heightx * 0.5) };
	{
		p1.setOrigin(0, 0);
		p2.setOrigin(0, 0);

		p1.setFillColor(Color::White);
		p2.setFillColor(Color::White);

		d.push_back(&p1);
		d.push_back(&p2);
	}

#ifdef useAudio
	bool playAudio = std::filesystem::exists("ov");
	if (playAudio) {
		mciSendStringA("open \"ov\" type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendStringA("play mp3 repeat", NULL, 0, NULL);
	}
#endif

	bool r = false, muted = false, pr = false;


	Vector2f pos[3]{ Vector2f((0.5 * widthx), (0.5 * heightx)) };
	/*
	pos[0]=current
	pos[1]=lastFrame
	pos[2]=calc*/
	circle.setPosition(pos[0]);
	int ballangle{ 0 };

	rballangle(&ballangle);

	timer pongTime, ballTime, fpsTime;
	pongTime.beginTime();
	ballTime.beginTime();
	fpsTime.beginTime();

	bool invert{ false },
		cst{ false },
		pinlines{ false },
		change{ true },
		keyboard{ true };
	bool failedc[]
	{ false, false };
	bool record[]
	{ false, false, false };

	std::atomic<double> fps;
	fps.store(0);

	//START RENDER THREAD
	window.setActive(false);
	std::thread renderThread(renderThreadp, &window, &d, &fps, &size[0], &record[0], &c3);

	while (window.isOpen()) {
		record[1] = Keyboard::isKeyPressed(Keyboard::Key::R);
		if (record[1] != record[2]) {
			if (record[1]) {
				record[0] = !record[0];
			}
			record[2] = record[1];
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //TO SLOW DOWN RUNTIME (ELSE IT RUNS TO FAST)
		keyboard = GetFocus() == windowhandle; //ENSURE WINDOW IS FOCUSED FOR KEYBOARD
		text.setString(
			std::string("RPS: ") + std::to_string((int)roundn(1.f / fpsTime.getTimer())) +
			std::string("\nFPS: ") + std::to_string((int)roundn(1.f / fps.load()))
		);
		fpsTime.beginTime();

		{
			/* HERE THE DRAWING GOES ON */
			p1.setPosition(40, pongHigth[0] - 60);
			p2.setPosition(widthx - 40 - 10, pongHigth[1] - 60);

			pos[2].x = cosn(ballangle) * ballspeed * ballTime.getTimer();
			pos[2].y = sinn(ballangle) * ballspeed * ballTime.getLast();
			ballTime.beginTime();

			if (invert) {
				pos[2].y *= -1;
				pos[2].x *= -1;
			}

			circle.move(pos[2]);


			pos[0] = circle.getPosition();

			failedc[0] = pos[0].x < 10;
			failedc[1] = pos[0].x > widthx - 10;
			if (failedc[0] || failedc[1]) {
				pos[0] = Vector2f((0.5 * widthx), (0.5 * heightx));
				rballangle(&ballangle);
				circle.setPosition(pos[0]);

				score[failedc[0] ? 0 : 1]--;
				change = true;
			}

			std::future<bool>
				fut = std::async(detectCollision,
					pos[0], p1.getPosition(), true),
				fut2 = std::async(detectCollision,
					pos[0], p2.getPosition(), false);
			fut.wait();
			if (!fut2._Is_ready())
				fut2.wait();
			cst = fut.get() || fut2.get();
			bool rt = cst && cst != pinlines;
			if (rt) {
				invert = !invert;
				score[invert ? 0 : 1]++;
				change = true;
			}

			if (pos[0].y < 10 || pos[0].y > heightx - 10 || rt) {
				circle.setPosition(pos[1]);
				ballangle = 360 - ballangle;
			}

			pinlines = cst;
		}
#ifdef useAudio
		{
			bool r = Keyboard::isKeyPressed(Keyboard::Key::M) && keyboard;
			if (r != pr && r) {
				if (playAudio)
					mciSendStringA(muted ? "resume mp3" : "pause mp3", NULL, 0, NULL);

				muted = !muted;
				change = true;
			}
			pr = r;
		}
#endif
		if (change) {
			tbmute.setString(createHelpText(muted, &score[0], record[0]).c_str());
			change = false;
		}
		{
			xhelp.pongmove(&pongHigth[0],
				Keyboard::isKeyPressed(Keyboard::Key::W) && keyboard,
				Keyboard::isKeyPressed(Keyboard::Key::S) && keyboard,
				pongTime.getTimer() * pongspeed);

			xhelp.pongmove(&pongHigth[1],
				Keyboard::isKeyPressed(Keyboard::Key::O) && keyboard,
				Keyboard::isKeyPressed(Keyboard::Key::L) && keyboard,
				pongTime.getLast() * pongspeed);

			pongTime.beginTime();
		}

		{
			if (Keyboard::isKeyPressed(Keyboard::Key::Escape) && keyboard) {
				window.close();
			}
			Event event;
			while (window.pollEvent(event)) {
				if (event.type == Event::Closed)
					window.close();
			}
		}
		pos[1] = pos[0];//STORE OLD POS IN STORE 1
	}
#ifdef useAudio
	if (playAudio)
		mciSendStringA("close mp3", NULL, 0, NULL);
#endif

	renderThread.join();

	return 0;
}

void renderThreadp(
	RenderWindow* window,
	std::vector<Drawable*>* d,
	std::atomic<double>* myfps,
	const int* size,
	bool* record = nullptr,
	Shape* dn = nullptr) {
	//PREPARE RECORD
	bool preco = (record != nullptr ? *record : false),
		featureRec = record != nullptr;
	std::queue<Texture> records;
	//END PREPARE RECORD

	//GL CONTEXT
	window->setActive(true);
	timer fpsTime;
	fpsTime.beginTime();

	const int fps[]{ 25, 400 };

	window->setFramerateLimit(fps[preco ? 0 : 1]);
	// the rendering loop
	while (window->isOpen())
	{
		if (featureRec) {
			if (preco != *record) {
				preco = *record;
				window->setFramerateLimit(fps[preco ? 0 : 1]);
			}
			if (preco) {
				if (10000 >= records.size() + 1) {
					sf::Vector2u windowSize = window->getSize();
					sf::Texture texture;
					texture.create(windowSize.x, windowSize.y);
					texture.update(*window);
					records.push(texture);
					//records.push_back(texture);
				}
				else
					*record = false;
			}
		}

		window->clear(Color::Black);
		myfps->store(fpsTime.getTimer());
		fpsTime.beginTime();
		//RENDER ALL
		for (unsigned int x{ 0 }; x < d->size(); x++) {
			window->draw(*(*d)[x]);
		}
		if (dn != nullptr) {
			dn->setFillColor(
				dn->getFillColor() == Color::Red ? Color::Blue : Color::Red
			);
			window->draw(*dn);
		}
		// end the current frame
		window->display();
	}

	//SAVE RECORDED IMAGES
	if (records.size() > 0) {
		const char rp[] = " Recorded Video";
		if (MessageBoxA(NULL,
			("We need to Save your " + std::to_string(records.size()) + rp +
				"\nThis can take some time").c_str(),
			gm, MB_OKCANCEL
			| MB_DEFBUTTON2 |
			MB_ICONINFORMATION |
			MB_TOPMOST) == IDCANCEL)
			return;

		size_t sizevid = size[0] * size[1] * sizeof(unsigned char);
		unsigned char* calced = (unsigned char*)malloc(sizevid * 3);
		VideoCapture vc;
		vc.Init(size[0], size[1], 24, 400000);
		while (records.size() > 0) {
			Image i = records.front().copyToImage();
			records.pop();
			const unsigned char* ux = i.getPixelsPtr();
			//RGBA TO RGB:
			for (size_t xr{ 0 }, xct{ 0 }, srx{ 0 }; xr < (sizevid * 4); xr++) {
				if (xct >= 3) {
					xct = 0;
					continue;
				}
				calced[srx] = ux[xr];
				xct++;
				srx++;
			}

			vc.AddFrame(calced);
		}
		free(calced);
		vc.Finish();

		//MOVE
		auto dp = std::string(getDocPath()),
			np = std::string(rndst(10));
	retryFs:

		if (std::filesystem::exists(dp + "\\" + np + ".mp4")) {
			np = rndst(16);
			goto retryFs;
		}

		std::string cfn = (dp + "\\" + np + ".mp4");
		const char tmpn[] = "recordTMP.mp4";
		bool ok = false;
		if (std::filesystem::exists(tmpn)) {
			ok = std::filesystem::copy_file(tmpn, cfn.c_str());
			if (ok)
				std::filesystem::remove(tmpn);
		}

		if (ok)
			MessageBoxA(NULL,
				("Saved all your " + std::to_string(records.size()) + rp +
					" to \"" + cfn + "\"").c_str(),
				gm, MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		else
			MessageBoxA(NULL,
				("Failue during Save of your" + std::string(rp)).c_str(),
				gm, MB_OK | MB_ICONHAND | MB_TOPMOST);
		//}
	}
}
