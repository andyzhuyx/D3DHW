#include "GameApp.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prevInst, LPSTR cmdLine, int showCmd) {
	Game game(hInst);

	if (!game.Init())
		return -1;

	return game.Run();
}