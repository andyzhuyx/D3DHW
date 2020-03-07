#pragma once
#include <queue>

class Mouse {
	friend class GameApp;

public:
	struct Delta {
		int x, y;
	};

	class Event {
	public:
		enum class Type {
			LPressed,
			RPressed,

			LReleased,
			RReleased,

			Move,
			Enter,
			Leave
		};
	private:
		Type _type;
		bool _leftPressed;
		bool _rightPressed;
		int _xPos;
		int _yPos;
	public:
		Event(Type type, Mouse& prevMouse);
		Type GetType()const;
		std::pair<int, int> GetPosition()const;
		int GetPosX()const;
		int GetPosY()const;
		bool IsLeftPressed()const;
		bool IsRightPressed()const;
	};

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	std::pair<int, int> GetPosition()const;
	Delta* ReadDelta();
	int GetPosX()const;
	int GetPosY()const;
	
	bool IsInWindow()const;
	bool IsLeftPressed()const;
	bool IsRightPressed()const;
	
	Event* ReadEvent();
	bool IsEventEmpty();
	void ClearBuffer();
	void PauseMouse();
	void RestartMouse();
	bool IsMousePaused();

private:
	void OnMouseMove(int x, int y);
	void OnMouseLeave();
	void OnMouseEnter();
	void OnDataDelta(int dx, int dy);
	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);

	void RestrictDeltaBuffer();
	void RestrictEventBuffer();

private:
	static const unsigned int BUFFER_SIZE = 16;
	int _xPos;
	int _yPos;
	bool _IsLeftPressed;
	bool _IsRightPressed;
	bool _IsInWindow;
	bool _IsMousePaused;
	std::queue<Delta> _DeltaBuffer;
	std::queue<Event> _EventBuffer;
};