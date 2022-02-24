#include "ofxChapterPlayer.h"
#include "ofLog.h"

using namespace std;
using namespace ofx::chapterplayer;

bool Chapter::load(const std::filesystem::path &name)
{
	return player_ ? player_->load(name) : false;
}
void Chapter::setSection(float start_time, float duration)
{
	start_time_ = start_time;
	duration_ = duration;
}
void Chapter::setDelay(float delay)
{
	delay_ = delay;
}

void Chapter::setLoop(bool loop)
{
	loop_times_ = loop ? -1 : 1;
}
void Chapter::setLoopTimes(int times)
{
	loop_times_ = times;
}
void Chapter::setRapCallback(std::function<bool(int,bool)> force_finish)
{
	rap_callback_ = force_finish; 
}
void Chapter::setFinishCallback(std::function<bool(bool)> replay)
{
	finish_callback_ = replay; 
}
void Chapter::play()
{
	stop();

	auto startBody = [this]() {
		is_during_delay_ = false;
		timer_.startLoop(duration_, loop_times_,
			// finish callback
			[this]() {
				for(auto &&s : sound_) {
					s.timer.disable();
					if(s.stop_on_finish) {
						s.player->stop();
					}
				}

				if(!finish_callback_(false)) {
					ofNotifyEvent(onFinish, this);
					return false;
				}
				for(auto &&s : sound_) {
					s.timer.restart();
				}
				return true;
			},
			// rap callback
			[this](int times, bool last) {
				for(auto &&s : sound_) {
					if(s.stop_on_every_rap) {
						s.player->stop();
						s.timer.disable();
					}
				}
				if(rap_callback_(times, last)) {
					if(!last) {
						for(auto &&s : sound_) {
							if(s.play_on_every_rap) {
								s.timer.restart();
							}
						}
					}
					return true;
				}
				finish();
				return false;
			}
		);
		for(auto &&s : sound_) {
			s.timer.restart();
		}
	};
	if(delay_ > 0) {
		is_during_delay_ = true;
		timer_.start(delay_, [startBody]() {
			startBody();
			return true;
		});
	}
	else {
		startBody();
	}
}
void Chapter::stop()
{
	timer_.disable();
	timer_.rewind();
	for(auto &&s : sound_) {
		s.timer.disable();
		if(s.stop_on_finish) {
			s.player->stop();
		}
	}
}
void Chapter::finish()
{
	stop();
	finish_callback_(true);
	ofNotifyEvent(onFinish, this);
}

void Chapter::update()
{
	if(player_) player_->update(start_time_+(is_during_delay_?0:timer_.getTime()));
}
void Chapter::draw(float x, float y, float w, float h) const
{
	if(player_) player_->draw(x,y,w,h);
}
float Chapter::getWidth() const
{
	return player_ ? player_->getWidth() : 0;
}
float Chapter::getHeight() const
{
	return player_ ? player_->getHeight() : 0;
}

void Chapter::setPaused(bool paused)
{
	timer_.setPaused(paused);
}
bool Chapter::isPaused() const
{
	return !timer_.isEnabled();
}
bool Chapter::isPlaying() const
{
	return timer_.isEnabled();
}


void Chapter::appendSound(std::shared_ptr<ofSoundPlayer> player, const SoundSettings &settings)
{
	sound_.emplace_back(Sound{});
	auto &sound = sound_.back();
	sound.player = player;
	player->setPan(settings.pan);
	player->setVolume(settings.volume);
	sound.timer.setup(settings.delay, [sound]() {
		sound.player->play();
		return false;
	});
	sound.stop_on_finish = settings.stop_on_finish;
	sound.play_on_every_rap = settings.play_on_every_rap;
	sound.stop_on_every_rap = settings.stop_on_every_rap;
}

std::shared_ptr<ofSoundPlayer> Chapter::appendSound(const std::filesystem::path &filepath, const SoundSettings &settings)
{
	auto sound = make_shared<ofSoundPlayer>();
	sound->load(filepath);
	appendSound(sound, settings);
	return sound;
}

// =========


ChapterPlayer::~ChapterPlayer()
{
	for(auto &&c : chapters_) {
		ofRemoveListener(c.second->onFinish, this, &ChapterPlayer::onChapterEnd);
	}
}

std::shared_ptr<Chapter> ChapterPlayer::addChapter(const std::string &name)
{
	auto ret = make_shared<Chapter>();
	auto result = chapters_.insert(make_pair(name, ret));
	if(!result.second) {
#ifdef TARGET_WIN32
		ofLogWarning(__FUNCSIG__) << "Chapter " << name << " already exists. Returning old one.";
#else
		ofLogWarning(__PRETTY_FUNCTION__) << "Chapter " << name << " already exists. Returning old one.";
#endif

		return result.first->second;
	}
	ofAddListener(ret->onFinish, this, &ChapterPlayer::onChapterEnd);
	return ret;
}
void ChapterPlayer::onChapterEnd(const void *chapter)
{
	auto result = find_if(begin(next_chapters_), end(next_chapters_), [chapter](const pair<shared_ptr<Chapter>, std::string> &next) {
		return chapter == next.first.get();
	});
	if(result == end(next_chapters_)) {
		return;
	}
	play(result->second);
}

std::shared_ptr<Chapter> ChapterPlayer::addChapter(const std::string &name, const ChapterSettings &s)
{
	auto ret = addChapter(name);
	ret->setSection(s.start, s.duration);
	ret->setDelay(s.delay);
	ret->setLoopTimes(s.loop_times);
	ret->setPlayer(s.player);
	return ret;
}
std::shared_ptr<Chapter> ChapterPlayer::getChapter(const std::string &name)
{
	auto result = chapters_.find(name);
	if(result == end(chapters_)) {
#ifdef TARGET_WIN32
		ofLogWarning(__FUNCSIG__) << "Chapter " << name << " not found.";
#else
		ofLogWarning(__PRETTY_FUNCTION__) << "Chapter " << name << " not found.";
#endif
		return nullptr;
	}
	return result->second;
}
void ChapterPlayer::setNext(std::shared_ptr<Chapter> from, const std::string &next)
{
	next_chapters_[from] = next;
}

std::string ChapterPlayer::getCurrentChapterName() const
{
	auto result = find_if(begin(chapters_), end(chapters_), [this](const pair<std::string, shared_ptr<Chapter>> &chapter) {
		return current_ == chapter.second;
	});
	if(result == end(chapters_)) {
		return "";
	}
	return result->first;
}

std::vector<std::string> ChapterPlayer::getAllChapterName() const
{
	std::vector<std::string> ret;
	ret.reserve(chapters_.size());
	for(auto &&s : chapters_) {
		ret.push_back(s.first);
	}
	return ret;
}

void ChapterPlayer::update()
{
	if(current_) current_->update();
}
void ChapterPlayer::draw(float x, float y, float w, float h) const
{
	if(current_) current_->draw(x,y,w,h);
}

float ChapterPlayer::getWidth() const
{
	return current_ ? current_->getWidth() : 0;
}
float ChapterPlayer::getHeight() const
{
	return current_ ? current_->getHeight() : 0;
}

void ChapterPlayer::play(const std::string &name)
{
	stop();
	current_ = getChapter(name);
	if(current_) current_->play();
}
void ChapterPlayer::stop()
{
	if(current_) current_->stop();
}
void ChapterPlayer::setPaused(bool paused)
{
	if(current_) current_->setPaused(paused);
}

void ChapterPlayer::next()
{
	if(current_) current_->finish();
}

