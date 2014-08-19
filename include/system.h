#ifndef SYSTEM_H
#define SYSTEM_H

template<typename T>
struct Vec2 {
	T x, y;
};

class Timer {
public:
	static float start, end, elapsed, frame, slice;
};

#endif