#pragma once

#include "ofEvents.h"
#include <functional>

using namespace std::chrono;

class Timer
{
public:
	using Callback = std::function<bool()>;
	using RapCallback = std::function<bool(int,bool)>;
	virtual ~Timer() {
		if(is_enabled_) {
			disable();
		}
	}
	void enable() {
		if(!is_enabled_) {
			ofAddListener(ofEvents().update, this, &Timer::update);
			is_enabled_ = true;
		}
	}
	void disable() {
		if(is_enabled_) {
			ofRemoveListener(ofEvents().update, this, &Timer::update);
			is_enabled_ = false;
		}
	}
	bool isEnabled() const { return is_enabled_; }
	void setup(float duration=-1, Callback callback=[](){return false;}) {
		loop_count_ = 0;
		duration_ = duration;
		callback_ = callback;
		rewind();
	}
	void setupLoop(float duration, int loops=-1, Callback finish_callback=[](){return false;}, RapCallback rap_callback=[](int loops, bool is_last){return true;}) {
		setup(duration, [&,loops,finish_callback,rap_callback]() {
			++loop_count_;
			bool is_last = loops > 0 && loop_count_ >= loops;
			if(!rap_callback(loop_count_, is_last)) {
				return false;
			}
			if(is_last) {
				return finish_callback();
			}
			return true;
		});
	}
	void start(float duration=-1, Callback callback=[](){return false;}) {
		setup(duration, callback);
		enable();
	}
	void startLoop(float duration, int loops=-1, Callback finish_callback=[](){return false;}, RapCallback rap_callback=[](int loops, bool is_last){return true;}) {
		setupLoop(duration, loops, finish_callback, rap_callback);
		restart();
	}
	void restart() {
		rewind();
		enable();
	}
	void rewind() {
		begin_time_ =
		current_ = system_clock::now();
	}
	void setTime(float seconds) {
		begin_time_ = current_ - milliseconds((long)(seconds*1000));
	}
	float getTime() const {
		return duration_cast<milliseconds>(current_-begin_time_).count()/1000.f;
	}
	float getPosition() const {
		return getTime()/(duration_ > 0 ? duration_ : 1.f);
	}
	void update(ofEventArgs&) {
		current_ = system_clock::now();
		float elapsed = getTime();
		if(duration_ >= 0 && duration_ <= elapsed) {
			float over = elapsed - duration_;
			if(callback_()) {
				rewind();
				setTime(over);
			}
			else {
				disable();
			}
		}
	}
	float getDuration() const { return duration_; }
	void setPaused(bool paused) {
		if(paused) {
			disable();
		}
		else if(!isEnabled()) {
			float time = getTime();
			rewind();
			setTime(time);
			enable();
		}
	}
protected:
	bool is_enabled_=false;
	system_clock::time_point begin_time_;
	float duration_=0;
	int loop_count_=0;
	system_clock::time_point current_;
	Callback callback_=[](){return false;};
};
