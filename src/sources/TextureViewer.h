#pragma once

#include "ofxChapterPlayer.h"
#include "ofTexture.h"
#include "ofImage.h"

class TextureViewer : public ofx::chapterplayer::PlayerBase
{
public:
	bool load(const std::string &filepath) { return ofLoadImage(tex_, filepath); }
	void setTexture(ofTexture tex) { tex_ = tex; }
	float getWidth() const { return tex_.getWidth(); }
	float getHeight() const { return tex_.getHeight(); }
	void draw(float x, float y, float w, float h) const {
		tex_.draw(x,y,w,h);
	}
protected:
	ofTexture tex_;
};
