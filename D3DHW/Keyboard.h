#pragma once
#include <queue>
#include <bitset>
class Keyboard {
public:
	class Event {
	public:
		enum Type {
			Pressed,
			Released
		};
	private:
		Type _Type;
		unsigned char _Char;

	public:
		Event(Type t, unsigned char c);
		bool IsPressed();
		bool IsReleased();
		unsigned char GetChar();
	};

public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	// Read Event
	Event* ReadEvent();
	// Clear Buffer
	void ClearBuffer();
	// On key Pressed
	void OnKeyPressed(unsigned char ch);
	// On key Released
	void OnKeyReleased(unsigned char ch);
	// Is buffer empty
	bool IsBufferEmpty();
	// Check if the key is pressed
	bool IsKeyPressed(unsigned char ch);

private:
	// buffer size
	const static unsigned int bufferSize = 16u;
	// Event queue
	std::queue<Event> buffer;
	// Keyboard key map
	std::bitset<256u> keyset;
};