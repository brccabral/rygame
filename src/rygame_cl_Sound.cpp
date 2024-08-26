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
        audio = new rl::Music;
        *(rl::Music *) audio = rl::LoadMusicStream(file);
        musics.push_back(this);
    }
    else
    {
        audio = new rl::Sound;
        *(rl::Sound *) audio = rl::LoadSound(file);
    }
}

rg::mixer::Sound::~Sound()
{
    if (isMusic)
    {
        musics.erase(std::remove(musics.begin(), musics.end(), this), musics.end());
        UnloadMusicStream(*(rl::Music *) audio);
        delete (rl::Music *) audio;
    }
    else
    {
        UnloadSound(*(rl::Sound *) audio);
        delete (rl::Sound *) audio;
    }
}

void rg::mixer::Sound::Play() const
{
    if (isMusic)
    {
        PlayMusicStream(*(rl::Music *) audio);
    }
    else
    {
        PlaySound(*(rl::Sound *) audio);
    }
}

void rg::mixer::Sound::Stop() const
{
    if (isMusic)
    {
        if (IsMusicStreamPlaying(*(rl::Music *) audio))
        {
            StopMusicStream(*(rl::Music *) audio);
        }
    }
    else
    {
        if (IsSoundPlaying(*(rl::Sound *) audio))
        {
            StopSound(*(rl::Sound *) audio);
        }
    }
}

void rg::mixer::Sound::SetVolume(const float value) const
{
    if (isMusic)
    {
        SetMusicVolume(*(rl::Music *) audio, value);
    }
    else
    {
        SetSoundVolume(*(rl::Sound *) audio, value);
    }
}

const char *rg::mixer::Sound::GetFilename() const
{
    return file;
}
