#include "Keyboard.h"

/*
	Event functions
*/

Keyboard::Event::Event(Keyboard::Event::Type type, unsigned char ch) 
	:
	_Type(type),
	_Char(ch)
{}

bool Keyboard::Event::IsPressed() {
	return (_Type == Pressed);
}

bool Keyboard::Event::IsReleased() {
	return (_Type == Released);
}

unsigned char Keyboard::Event::GetChar() {
	return _Char;
}

/*
	Keyboard functions
*/
Keyboard::Event* Keyboard::ReadEvent() {
	if (buffer.empty()) {
		return nullptr;
	}
	else {
		Event* tempPtr = &buffer.front();
		buffer.pop();
		return tempPtr;
	}
}

void Keyboard::ClearBuffer() {
	buffer = std::queue<Event>();
}

void Keyboard::OnKeyPressed(unsigned char ch) {
	keyset[ch] = true;
	buffer.push(Event(Event::Type::Pressed, ch));
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}

void Keyboard::OnKeyReleased(unsigned char ch) {
	keyset[ch] = false;
	buffer.push(Event(Event::Type::Released, ch));
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}

bool Keyboard::IsBufferEmpty() {
	return buffer.empty();
}

bool Keyboard::IsKeyPressed(unsigned char ch) {
	return keyset[ch];
}