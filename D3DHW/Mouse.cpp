#include "Mouse.h"

/*
	Event Inner Class
*/

Mouse::Event::Event(Mouse::Event::Type type, Mouse& prev) 
	:
	_type(type),
	_leftPressed(false),
	_rightPressed(false),
	_xPos(prev._xPos),
	_yPos(prev._yPos)
{}

Mouse::Event::Type Mouse::Event::GetType()const {
	return _type;
}

std::pair<int, int> Mouse::Event::GetPosition()const {
	return { _xPos, _yPos };
}

int Mouse::Event::GetPosX()const {
	return _xPos;
}

int Mouse::Event::GetPosY()const {
	return _yPos;
}

bool Mouse::Event::IsLeftPressed()const {
	return _leftPressed;
}

bool Mouse::Event::IsRightPressed()const {
	return _rightPressed;
}


/*
	Mouse Class Functions
*/
std::pair<int, int> Mouse::GetPosition()const {
	return { _xPos, _yPos };
}

Mouse::Delta* Mouse::ReadDelta() {
	if (_DeltaBuffer.empty()) {
		return nullptr;
	}
	else {
		Delta* temp = &_DeltaBuffer.front();
		_DeltaBuffer.pop();
		return temp;
	}
}

int Mouse::GetPosX()const {
	return _xPos;
}

int Mouse::GetPosY()const {
	return _yPos;
}

bool Mouse::IsInWindow()const {
	return _IsInWindow;
}

bool Mouse::IsLeftPressed()const {
	return _IsLeftPressed;
}

bool Mouse::IsRightPressed()const {
	return _IsRightPressed;
}

Mouse::Event* Mouse::ReadEvent() {
	if (_EventBuffer.empty()) {
		return nullptr;
	}
	else {
		Event* tempPtr = &_EventBuffer.front();
		_EventBuffer.pop();
		return tempPtr;
	}
}

bool Mouse::IsEventEmpty() {
	return _EventBuffer.empty();
}

void Mouse::ClearBuffer() {
	_EventBuffer = std::queue<Event>();
	_DeltaBuffer = std::queue<Delta>();
}

void Mouse::PauseMouse() {
	_IsMousePaused = false;
}

void Mouse::RestartMouse() {
	_IsMousePaused = true;
}

bool Mouse::IsMousePaused() {
	return _IsMousePaused;
}

void Mouse::OnMouseMove(int x, int y) {
	_xPos = x;
	_yPos = y;

	_EventBuffer.push(Event(Event::Type::Move, *this));
	RestrictEventBuffer();
}

void Mouse::OnMouseLeave() {
	_IsMousePaused = true;
	_IsInWindow = false;

	_EventBuffer.push(Event(Event::Type::Leave, *this));
	RestrictEventBuffer();
}

void Mouse::OnMouseEnter() {
	_IsMousePaused = false;
	_IsInWindow = true;
	_EventBuffer.push(Event(Event::Type::Enter, *this));
	RestrictEventBuffer();
}

void Mouse::OnDataDelta(int dx, int dy) {
	_DeltaBuffer.push(Delta({ dx,dy }));
	RestrictDeltaBuffer();
}

void Mouse::OnLeftPressed(int x, int y) {
	_IsLeftPressed = true;
	_EventBuffer.push(Event(Event::Type::LPressed, *this));
	RestrictEventBuffer();
}

void Mouse::OnLeftReleased(int x, int y) {
	_IsLeftPressed = false;
	_EventBuffer.push(Event(Event::Type::LReleased, *this));
	RestrictEventBuffer();
}

void Mouse::OnRightPressed(int x, int y) {
	_IsRightPressed = true;
	_EventBuffer.push(Event(Event::Type::RPressed, *this));
	RestrictEventBuffer();
}

void Mouse::OnRightReleased(int x, int y) {
	_IsRightPressed = false;
	_EventBuffer.push(Event(Event::Type::RReleased, *this));
	RestrictEventBuffer();
}

void Mouse::RestrictDeltaBuffer() {
	while (_DeltaBuffer.size() > Mouse::BUFFER_SIZE) {
		_DeltaBuffer.pop();
	}
}

void Mouse::RestrictEventBuffer() {
	while (_EventBuffer.size() > Mouse::BUFFER_SIZE) {
		_EventBuffer.pop();
	}
}