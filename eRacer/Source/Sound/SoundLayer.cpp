#include "SoundLayer.h"

namespace Sound {

SoundLayer* SoundLayer::m_pGlobalSoundLayer = NULL;

const string SoundLayer::SOUND_FOLDER("Resources/Sounds/");


SoundFx::~SoundFx()
{
	if ((SoundLayer::GetInstance()->m_isShutdown == false) && (NULL!= soundsample)) // check if FMOD is still alive first
	{
		FSOUND_Sample_Free(soundsample); 
		soundsample = NULL;
	}
}


SoundLayer::SoundLayer()
{
}

SoundLayer::~SoundLayer()
{
	if (NULL != m_pGlobalSoundLayer) {
		delete m_pGlobalSoundLayer;
		m_pGlobalSoundLayer = NULL;
	}
}

int SoundLayer::mydebug()
{
	return FSOUND_GetChannelsPlaying();
	// return FSOUND_GetCPUUsage();
	//return FSOUND_GetMixer();
}

void SoundLayer::LoadSoundFx(const string& filename, SoundFx* samp)
{
	if (NULL != samp->soundsample) {
		FSOUND_Sample_Free(samp->soundsample);
		samp->soundsample = NULL;
	}

	//Can't dynamically change from a 2D sound to a 3D sound, so must be set here
	unsigned int mode = 0;
	if (samp->isLooping)
		mode |= FSOUND_LOOP_NORMAL;
	else
		mode |= FSOUND_LOOP_OFF;
	
	if (!samp->is3D)
		mode |= FSOUND_2D;
	else
		mode |= FSOUND_HW3D;


	samp->soundsample = FSOUND_Sample_Load(FSOUND_FREE, (SOUND_FOLDER+filename).c_str(), mode, 0, 0);
	/*if ((!samp->soundsample)&&(samp->is3D)) {//If hardware loading failed, force software
		mode ^= FSOUND_HW3D; 
		samp->soundsample = FSOUND_Sample_Load(FSOUND_FREE, filename.c_str(), mode, 0, 0);
	}*/

	samp->channel = FSOUND_PlaySoundEx(FSOUND_FREE, samp->soundsample, NULL, TRUE);
	
	FSOUND_SetFrequency(samp->channel, samp->pitch);
	FSOUND_SetVolume(samp->channel, samp->volume);
	FSOUND_SetPriority(samp->channel, samp->priority);

	if (samp->is3D) {
		FSOUND_3D_SetMinMaxDistance(samp->channel, samp->minDist, samp->maxDist);
		FSOUND_3D_SetAttributes( samp->channel, (float*)&samp->position, (float*)&samp->velocity);
	}

    if (!samp->isPaused)
		FSOUND_SetPaused(samp->channel, FALSE);
}

void SoundLayer::UpdateSoundFx(SoundFx* samp)
{
	if (NULL == samp->soundsample)
		return;

	FSOUND_SetFrequency(samp->channel, samp->pitch);
	FSOUND_SetPriority(samp->channel, samp->priority);

	if (samp->is3D) {
		FSOUND_Sample_SetMinMaxDistance(samp->soundsample, samp->minDist, samp->maxDist);
		FSOUND_3D_SetAttributes( samp->channel, (float*)&samp->position, (float*)&samp->velocity);
	}

	if (samp->isLooping)
		FSOUND_Sample_SetMode(samp->soundsample, FSOUND_LOOP_NORMAL);
	else
		FSOUND_Sample_SetMode(samp->soundsample, FSOUND_LOOP_OFF);

	char pause = FALSE;
	if (samp->isPaused)
		pause = TRUE;
	FSOUND_SetPaused(samp->channel, pause);
}

void SoundLayer::StopSounds()
{
	FSOUND_StopSound(FSOUND_ALL);
}

void SoundLayer::ResetSound(SoundFx* samp)
{
	FSOUND_SetCurrentPosition(samp->channel,0);
}

void SoundLayer::PlaySoundFx(SoundFx* samp)
{
	if (NULL == samp->soundsample)
		return;

	if (samp->isLooping) //Only meant for one shot sounds
		return;

	samp->channel = FSOUND_PlaySoundEx(FSOUND_FREE, samp->soundsample, NULL, TRUE);
	FSOUND_SetFrequency(samp->channel, samp->pitch);
	FSOUND_SetVolume(samp->channel, samp->volume);
	FSOUND_SetPriority(samp->channel, samp->priority);

	if (samp->is3D) {
		FSOUND_3D_SetMinMaxDistance(samp->channel, samp->minDist, samp->maxDist);
		FSOUND_3D_SetAttributes( samp->channel, (float*)&samp->position, (float*)&samp->velocity);
	}
	
	FSOUND_SetPaused(samp->channel, FALSE);
}

int SoundLayer::Init()
{
	/*char* str;
	if(!FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND))
		str = FMOD_ErrorString(FSOUND_GetError());
	if(!FSOUND_SetDriver(0))
		str = FMOD_ErrorString(FSOUND_GetError());
	if(!FSOUND_SetMixer(FSOUND_MIXER_AUTODETECT));
		str = FMOD_ErrorString(FSOUND_GetError());

	if (FSOUND_GetVersion() < FMOD_VERSION)
		return -1; //Outdated DLL*/

	unsigned int flags = 0;
	/*flags |= FSOUND_INIT_DSOUND_DEFERRED;
	flags |= FSOUND_INIT_DONTLATENCYADJUST;
	flags |= FSOUND_INIT_STREAM_FROM_MAIN_THREAD;*/

	//FSOUND_SetMaxHardwareChannels(2);
	//FSOUND_SetMinHardwareChannels(0);
	//FSOUND_SetMixer(FSOUND_MIXER_QUALITY_FPU);
	//FSOUND_SetMixer(FSOUND_MIXER_MAX);
	// FSOUND_SetMixer(FSOUND_MIXER_QUALITY_MMXP5);
	// FSOUND_SetMixer(FSOUND_MIXER_QUALITY_MMXP6);
	FSOUND_SetBufferSize(90);

	if (!FSOUND_Init(44100, 32, flags))
    {
		return -1; //Failed to Initialize
        printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));
    }

	m_isShutdown = false;

	return 0;
}

int SoundLayer::SetOrientation3D(const Point3& listenerPos, const Vector3& listenerVel, const Vector3& atVector, const Vector3& upVector)
{
	float pos[3] = { listenerPos.x, listenerPos.y, listenerPos.z };
	float vel[3] = { listenerVel.x, listenerVel.y, listenerVel.z };
	FSOUND_3D_Listener_SetAttributes(pos, vel, 
		atVector.x, atVector.y, atVector.z, 
		upVector.x, upVector.y, upVector.z);

	return 0;
}

int SoundLayer::SetOrientation3DB(const Point3& listenerPos, const Vector3& listenerVel, const Vector3& atVector, const Vector3& upVector, int listener, int total)
{
	float pos[3] = { listenerPos.x, listenerPos.y, listenerPos.z };
	float vel[3] = { listenerVel.x, listenerVel.y, listenerVel.z };

	FSOUND_3D_Listener_SetCurrent( listener, total );
	FSOUND_3D_Listener_SetAttributes(pos, vel, 
		atVector.x, atVector.y, atVector.z, 
		upVector.x, upVector.y, upVector.z);

	return 0;
}

int SoundLayer::Update()
{
	//Update needs to be called each frame for 3D sound to change pitch.
	FSOUND_Update();
	return 0;
}

int SoundLayer::Shutdown()
{
	m_isShutdown = true;
	FSOUND_Close();

	return 0;
}

//Potentially Obsolete
int SoundLayer::PlaySound2D(const string& name)
{
	FilenameCache::iterator sound;
	if ((sound = m_SoundCache2D.find(name)) == m_SoundCache2D.end()) { //Cache Miss
		FSOUND_SAMPLE  *soundClip = NULL;
		soundClip = FSOUND_Sample_Load(FSOUND_FREE, name.c_str(), FSOUND_HW2D, 0, 0);
		if (!soundClip)
			return -1; //Failed to load
		
		sound = m_SoundCache2D.insert(make_pair(name, soundClip)).first;
	}
	int chan = FSOUND_PlaySound(FSOUND_FREE, sound->second);
	return 0;
}

};