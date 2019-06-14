#include "ofApp.h"
#include "TimeViewer.h"

using namespace std;

//--------------------------------------------------------------
void ofApp::setup(){
	auto view = make_shared<TimeViewer>();
	player_ = make_shared<ofxChapterPlayer>();
	
	auto idle = player_->addChapter("idle", {0,1,0,-1,view});
	{
		ofx::chapterplayer::Chapter::SoundSettings ss;
		ss.delay = 0;
		ss.stop_on_finish = true;
		ss.play_on_every_rap = false;
		ss.stop_on_every_rap = false;
		auto sound = idle->appendSound("idle_loop.wav", ss);
		sound->setLoop(true);
	}
	auto play = player_->addChapter("play", {2,3,0,1,view});
	{
		ofx::chapterplayer::Chapter::SoundSettings ss;
		ss.delay = 0;
		ss.stop_on_finish = false;
		ss.play_on_every_rap = true;
		ss.stop_on_every_rap = false;
		play->appendSound("play.wav", ss);
	}
	auto end = player_->addChapter("end", {10,2,2,3,view});
	{
		ofx::chapterplayer::Chapter::SoundSettings ss;
		ss.delay = 1;
		ss.stop_on_finish = true;
		ss.play_on_every_rap = true;
		ss.stop_on_every_rap = true;
		end->appendSound("end.wav", ss);
	}
	
	player_->setNext(idle, "play");
	player_->setNext(play, "end");
	player_->setNext(end, "idle");
}

//--------------------------------------------------------------
void ofApp::update(){
	player_->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	player_->draw(100,100);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key) {
		case '1': player_->play("idle"); break;
		case '2': player_->play("play"); break;
		case '3': player_->play("end"); break;
		case OF_KEY_RIGHT: player_->next(); break;
		case OF_KEY_RETURN: player_->stop(); break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
