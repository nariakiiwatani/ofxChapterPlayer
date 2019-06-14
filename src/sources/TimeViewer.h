#pragma once

#include "ofxChapterPlayer.h"

class TimeViewer : public ofx::chapterplayer::PlayerBase
{
public:
	void update(float current_time) { time_ = current_time; }
	float getWidth() const { return 1; }
	float getHeight() const { return 1; }
	void draw(float x, float y, float w, float h) const {
		ofDrawBitmapStringHighlight(ofToString(time_,2), x,y);
	}
private:
	float time_;
};

