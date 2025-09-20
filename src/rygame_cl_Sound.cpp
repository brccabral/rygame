#include <algorithm>
#include "rygame.hpp"
#include "rygame_cl_Rygame.hpp"


extern Rygame rygame;

rg::mixer::Sound::Sound(const char *file, const bool isMusic) : isMusic(isMusic), file(file)
{
    if (!rygame.isSoundInit)
    {
        rl::InitAudioDevice();
        rygame.isSoundInit = rl::IsAudioDeviceReady();
    }
    if (isMusic)
    {
        audio.music = rl::Music(rl::LoadMusicStream(file));
        rygame.musics.push_back(this);
    }
    else
    {
        audio.sound = rl::Sound(rl::LoadSound(file));
    }
}

rg::mixer::Sound::~Sound()
{
    if (isMusic)
    {
        std::erase(rygame.musics, this);
        UnloadMusicStream(audio.music);
    }
    else
    {
        UnloadSound(audio.sound);
    }
}

// TODO - param to allow repetitions
void rg::mixer::Sound::Play() const
{
    if (isMusic)
    {
        PlayMusicStream(audio.music);
    }
    else
    {
        PlaySound(audio.sound);
    }
}

void rg::mixer::Sound::Stop() const
{
    if (isMusic)
    {
        if (IsMusicStreamPlaying(audio.music))
        {
            StopMusicStream(audio.music);
        }
    }
    else
    {
        if (IsSoundPlaying(audio.sound))
        {
            StopSound(audio.sound);
        }
    }
}

void rg::mixer::Sound::SetVolume(const float value) const
{
    if (isMusic)
    {
        SetMusicVolume(audio.music, value);
    }
    else
    {
        SetSoundVolume(audio.sound, value);
    }
}

const char *rg::mixer::Sound::GetFilename() const
{
    return file;
}
