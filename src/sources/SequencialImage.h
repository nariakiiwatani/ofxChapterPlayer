#pragma once

#include "TextureViewer.h"
#include "ofFileUtils.h"

class SequencialImageViewer : public TextureViewer
{
public:
	void setPreload(bool preload) { preload_ = preload; }
	bool load(const std::string &directorypath) {
		images_.clear();
		ofDirectory dir = ofDirectory(directorypath).getSorted();
		files_ = dir.getFiles();
		if(!preload_) {
			images_.resize(files_.size());
			return true;
		}
		images_.reserve(files_.size());
		for(auto &&f : files_) {
			ofTexture tex;
			if(ofLoadImage(tex, f.path())) {
				images_.push_back(tex);
			}
		}
		if(images_.empty()) {
			return false;
		}
		tex_ = images_.front();
		return true;
	}
	std::size_t size() const { return images_.size(); }
	void clear() { images_.clear(); }
	void setFrameRate(float fps) { fps_ = fps; }
	void update(float current_time) {
		int index = current_time*fps_;
		if(!preload_) {
			while(!images_[index].isAllocated() && files_.size() > index) {
				auto &f = files_[index];
				if(!ofLoadImage(images_[index], f.path())) {
					images_.erase(images_.begin()+index);
					files_.erase(files_.begin()+index);
				}
			}
		}
		tex_ = images_[index];
	}
protected:
	bool preload_=true;
	float fps_=30;
	std::vector<ofFile> files_;
	std::vector<ofTexture> images_;
};
