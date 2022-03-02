#pragma once

#include "Timer.h"
#include "ofGraphicsBaseTypes.h"
#include "ofSoundPlayer.h"
#include <map>

namespace ofx {
namespace chapterplayer {

class PlayerBase : public ofBaseDraws
{
public:
	virtual bool load(const std::filesystem::path &name) { return true; }
	virtual void update(float current_time){}
};

class Chapter : public ofBaseUpdates, public ofBaseDraws
{
public:
	bool load(const std::filesystem::path &name);
	
	void setPlayer(std::shared_ptr<PlayerBase> player) { player_ = player; }
	template<typename T> std::shared_ptr<T> getPlayer() { return std::dynamic_pointer_cast<T>(player_); }
	
	void update() override;
	void draw() const { draw(0,0); }
	void draw(float x, float y, float w, float h) const override;
	using ofBaseDraws::draw;
	
	float getWidth() const override;
	float getHeight() const override;
	
	void play();
	void stop();
	void finish();
	void setPaused(bool paused);
	bool isPaused() const;
	bool isPlaying() const;
	float getDuration() const { return duration_; }
	float getPosition() const { return timer_.getPosition(); }
	
	void setSection(float start_time, float duration);
	void setDelay(float delay);
	void setLoop(bool loop);
	void setLoopTimes(int times);
	void setRapCallback(std::function<bool(int loop_count, bool is_last)> force_finish);
	void setFinishCallback(std::function<bool(bool forced)> replay);
	ofEvent<void> onFinish;
	
	struct SoundSettings {
		SoundSettings(){}
		float delay=0;
		float volume=1;
		float fadein=0, fadeout=0;
		float start_time=0;
		float pan=0.5f;
		bool multiplay=false;
		bool stop_on_finish=false;
		bool play_on_every_rap=true;
		bool stop_on_every_rap=false;
	};
	void appendSound(std::shared_ptr<ofSoundPlayer> player, const SoundSettings &settings=SoundSettings());
	std::shared_ptr<ofSoundPlayer> appendSound(const std::filesystem::path &filepath, const SoundSettings &settings=SoundSettings());
	
protected:
	std::shared_ptr<PlayerBase> player_;
	Timer timer_;
	float start_time_;
	float duration_;
	int loop_times_=1;
	float delay_=0;
	bool is_during_delay_=false;

	std::function<bool(int,bool)> rap_callback_=[](int,bool){return true;};
	std::function<bool(bool)> finish_callback_=[](bool){return false;};
	
	struct Sound {
		~Sound();
		std::shared_ptr<ofSoundPlayer> player;
		Timer timer;
		float volume=1;
		float fadein=0, fadeout=0;
		float start_time=0;
		bool stop_on_finish;
		bool stop_on_every_rap;
		bool play_on_every_rap;
		void play();
		void stop();
		void updateFade(ofEventArgs&);
	private:
		void addListener();
		void removeListener();
		bool fade_active=false;
		float fade_value=1;
		float fade_speed=0;
	};
	std::vector<Sound> sound_;
};

class ChapterPlayer : public ofBaseDraws
{
public:
	virtual ~ChapterPlayer();
	struct ChapterSettings {
		float start, duration, delay;
		int loop_times;
		std::shared_ptr<PlayerBase> player;
	};
	std::shared_ptr<Chapter> addChapter(const std::string &name);
	std::shared_ptr<Chapter> addChapter(const std::string &name, const ChapterSettings &settings);
	std::shared_ptr<Chapter> getChapter(const std::string &name);
	void setNext(std::shared_ptr<Chapter> from, const std::string &next);
	std::string getCurrentChapterName() const;
	std::vector<std::string> getAllChapterName() const;
	
	void update();
	virtual void draw() const { draw(0,0); }
	virtual void draw(float x, float y, float w, float h) const override;
	using ofBaseDraws::draw;
	
	float getWidth() const override;
	float getHeight() const override;
	
	void play(const std::string &name);
	void stop();
	void setPaused(bool paused);
	void next();
protected:
	std::map<std::string, std::shared_ptr<Chapter>> chapters_;
	std::map<std::shared_ptr<Chapter>, std::string> next_chapters_;
	std::shared_ptr<Chapter> current_;
	void onChapterEnd(const void *chapter);
};
}}
using ofxChapterPlayer = ofx::chapterplayer::ChapterPlayer;
