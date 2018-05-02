#include "SoundManager.h"
#include "SimpleAudioEngine.h"
#include "ResourcePath.h"

using namespace CocosDenshion;

static SoundManager* g_sharedManager = NULL;

SoundManager* SoundManager::getInstance()
{
	//获取instance;
    if(!g_sharedManager)
    {
        g_sharedManager = new SoundManager();
        g_sharedManager->init();

    }
    
    return g_sharedManager;
}

Ref* SoundManager::init()
{
	_isSoundOn = UserDefault::getInstance()->getBoolForKey("issoundon", true);
	_isMusicOn = UserDefault::getInstance()->getBoolForKey("ismusicon", true);
	_isMusicPlayed = false;
    return this;
}

void SoundManager::loadSounds()
{
    // TODO :插入soundid列表
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_BUTTON, ResourcePath::makeSoundPath("button.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_CAIJI, ResourcePath::makeSoundPath("caiji.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_KANSHU, ResourcePath::makeSoundPath("kanshu.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_WAJUE, ResourcePath::makeSoundPath("wajue.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_ATTACK, ResourcePath::makeSoundPath("attack.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_HURT0, ResourcePath::makeSoundPath("hurt0.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_HURT1, ResourcePath::makeSoundPath("hurt1.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_BUYOK, ResourcePath::makeSoundPath("buyok.mp3")));
	//插入背景音乐列表
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_START, ResourcePath::makeSoundPath("start.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_HOME, ResourcePath::makeSoundPath("home.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_MAP, ResourcePath::makeSoundPath("map.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_ENTER_MAPADDR, ResourcePath::makeSoundPath("mapaddr.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_FIGHT_0, ResourcePath::makeSoundPath("fight0.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_FIGHT_1, ResourcePath::makeSoundPath("fight1.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_FIGHT_2, ResourcePath::makeSoundPath("fight2.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_FIGHT_3, ResourcePath::makeSoundPath("fight3.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_DEATH, ResourcePath::makeSoundPath("death.mp3")));


	//预加载音效
	SimpleAudioEngine::getInstance()->preloadEffect(ResourcePath::makeSoundPath("button.mp3").c_str());
	SimpleAudioEngine::getInstance()->preloadEffect(ResourcePath::makeSoundPath("caiji.mp3").c_str());
	SimpleAudioEngine::getInstance()->preloadEffect(ResourcePath::makeSoundPath("kanshu.mp3").c_str());
	SimpleAudioEngine::getInstance()->preloadEffect(ResourcePath::makeSoundPath("wajue.mp3").c_str());
	SimpleAudioEngine::getInstance()->preloadEffect(ResourcePath::makeSoundPath("attack.mp3").c_str());
	SimpleAudioEngine::getInstance()->preloadEffect(ResourcePath::makeSoundPath("hurt0.mp3").c_str());
	SimpleAudioEngine::getInstance()->preloadEffect(ResourcePath::makeSoundPath("hurt1.mp3").c_str());
	SimpleAudioEngine::getInstance()->preloadEffect(ResourcePath::makeSoundPath("buyok.mp3").c_str());
}

void SoundManager::unloadSounds()
{
	_soundIds.clear();
	_musicIds.clear();
}

int SoundManager::playSound(int soundId, bool isloop)
{

	if(!_isSoundOn)
	{
	    return -1;
	}
    
    SoundIdMapType::iterator it = _soundIds.find(soundId);
    if (it == _soundIds.end()) {
        return -1;
    }
    
    std::string soundName = it->second;
    SimpleAudioEngine* soundEngine = SimpleAudioEngine::getInstance();
	return soundEngine->playEffect(soundName.c_str(), isloop);
}

bool SoundManager::getSoundIsOn()
{
	return _isSoundOn;
}

bool SoundManager::getMusicPlayed()
{
	return _isMusicPlayed;
}

void SoundManager::playBackMusic(const char * filename)
{
	if (_isMusicOn)
    {
		_isMusicPlayed = true;
        SimpleAudioEngine::getInstance()->playBackgroundMusic(filename,true);
    }
}

void SoundManager::playBackMusic(int musicid)
{
	if (_isMusicOn)
	{
		MusicIdMapType::iterator it = _musicIds.find(musicid);
		if (it == _musicIds.end()) {
			return;
		}

		_isMusicPlayed = true;

		std::string musciName = it->second;
		SimpleAudioEngine::getInstance()->playBackgroundMusic(musciName.c_str(), true);
	}
}

void SoundManager::stopBackMusic()
{
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}


void SoundManager::pauseBackMusic()
{
	SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void SoundManager::resumeBackMusic()
{
	if (_isMusicOn)
	{
		SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	}
}

void SoundManager::setIsMusicOn(bool isMusicOn)
{
	_isMusicOn = isMusicOn;
	UserDefault::getInstance()->setBoolForKey("ismusicon", _isMusicOn);
}

void SoundManager::setIsSoundOn(bool isSoundOn)
{
    _isSoundOn = isSoundOn;
	UserDefault::getInstance()->setBoolForKey("issoundon", _isSoundOn);
}

void SoundManager::stopSound(int soundId)
{
	if (_isSoundOn)
		SimpleAudioEngine::getInstance()->stopEffect(soundId);

}
void SoundManager::stopAllEffectSound()
{
	if (_isSoundOn)
		SimpleAudioEngine::getInstance()->stopAllEffects();
}
void SoundManager::pauseAllEffectSound()
{
	if (_isSoundOn)
		SimpleAudioEngine::getInstance()->pauseAllEffects();
}
void SoundManager::resumeAllEffectSound()
{
	if (_isSoundOn)
		SimpleAudioEngine::getInstance()->resumeAllEffects();
}

float SoundManager::getVolume()
{
    return SimpleAudioEngine::getInstance()->getEffectsVolume();
}

void SoundManager::setVolume(float volume)
{
    SimpleAudioEngine* soundEngine = SimpleAudioEngine::getInstance();
    soundEngine->setEffectsVolume(volume);
    soundEngine->setBackgroundMusicVolume(volume);
}

void SoundManager::saveVolume()
{
    UserDefault::getInstance()->setFloatForKey("volume", getVolume());
}

int SoundManager::getSaveVolume()
{
    return UserDefault::getInstance()->getFloatForKey("volume", getVolume());
}
