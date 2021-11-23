#include <SFML/Graphics.hpp> /*MUß OBEN SEIN*/
#include <Windows.h>
#include <string>
#include <vector>
#include <xhelper.h>
#include <future>

#define pongspeed 400
#define ballspeed 421
#define appguid "{03878e53-d462-4c67-9b36-ec7192f49a8b}" /*POWERED BY https://www.guidgen.com/*/

using namespace sf;

std::string createHelpText(bool muted, int *score) {
	return std::string("Keys:\n") + std::string(muted ? "Music" : "Mute") +
		std::string(": M  \n\nPlayer 1:\n\tUp: W\n\tDown: S\n\nPlayer 2:\n\tUp: O\n\tDown: L\n\nExit: ESC\n\nScore:\n\tP1: ")
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
	const int size[]{ 960, 640 };/*HEIGHT, WIDTH*/
#define widthx (float)size[0]
#define heightx (float)size[1]

	xhelpers xhelp(&size[0]);
	std::vector<Drawable*> d;

	bool ui{ true };

	RenderWindow window(VideoMode(size[0], size[1]), L"Pong - \uAC8C\uC784");
	HWND windowhandle = window.getSystemHandle();


	/*RANDOM INIT*/
	rInit();

	if (shouldClose(appguid)) {
		MessageBoxA(windowhandle, "OOPS, ONE INSTANCE OF \"Pong\" already running,\nClose this Version before start a new", "Pong", MB_OK | MB_ICONHAND | MB_TOPMOST);
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

	window.setFramerateLimit(420);

	mciSendStringA("open \"ov\" type mpegvideo alias mp3", NULL, 0, NULL);
	mciSendStringA("play mp3 repeat", NULL, 0, NULL);

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

	bool invert{ false }, cst{ false }, pinlines{ false }, change{ true };
	bool failedc[]{ false, false };

	while (window.isOpen()) {
		window.clear(Color::Black);
		text.setString(std::string("FPS: ") + std::to_string((int)roundn(1.f / fpsTime.getTimer())));
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

				score[failedc[0]?0:1]--;
				change = true;
			}

			std::future<bool>
				fut = std::async(detectCollision, pos[0], p1.getPosition(), true),
				fut2 = std::async(detectCollision, pos[0], p2.getPosition(), false);
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

			//RENDER ALL
			for (unsigned int x{ 0 }; x < d.size(); x++) {
				window.draw(*d[x]);
			}
		}

		{
			bool r = Keyboard::isKeyPressed(Keyboard::Key::M);
			if (r != pr && r) {
				mciSendStringA(muted ? "resume mp3" : "pause mp3", NULL, 0, NULL);
				muted = !muted;
				change = true;
			}
			if (change) {
				tbmute.setString(createHelpText(muted, &score[0]).c_str());
				change = false;
			}
			pr = r;
		}

		window.display();

		{
			xhelp.pongmove(&pongHigth[0], Keyboard::isKeyPressed(Keyboard::Key::W), Keyboard::isKeyPressed(Keyboard::Key::S), pongTime.getTimer() * pongspeed);
			xhelp.pongmove(&pongHigth[1], Keyboard::isKeyPressed(Keyboard::Key::O), Keyboard::isKeyPressed(Keyboard::Key::L), pongTime.getLast() * pongspeed);
			pongTime.beginTime();
		}

		{
			if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) {
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

	mciSendStringA("close mp3", NULL, 0, NULL);

	return 0;
}

