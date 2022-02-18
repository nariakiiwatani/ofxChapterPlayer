#pragma once

#include "ofxChapterPlayer.h"
#include "ofTexture.h"
#include "ofImage.h"

class TextureViewer : public ofx::chapterplayer::PlayerBase, public ofBaseHasTexture
{
public:
	bool load(const std::filesystem::path &filepath) override { return ofLoadImage(tex_, filepath); }
	void setTexture(ofTexture tex) { tex_ = tex; }
	float getWidth() const override { return tex_.getWidth(); }
	float getHeight() const override { return tex_.getHeight(); }
	void draw(float x, float y, float w, float h) const override {
		tex_.draw(x,y,w,h);
	}
	ofTexture & getTexture() override { return tex_; }
	const ofTexture & getTexture() const override { return tex_; }
	void setUseTexture(bool bUseTex) override {}
	bool isUsingTexture() const override { return true; }
protected:
	ofTexture tex_;
};
