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
        audio = std::make_shared<rl::Music>(rl::LoadMusicStream(file));
        rygame.musics.push_back(this);
    }
    else
    {
        audio = std::make_shared<rl::Sound>(rl::LoadSound(file));
    }
}

rg::mixer::Sound::~Sound()
{
    if (isMusic)
    {
        rygame.musics.erase(
                std::remove(rygame.musics.begin(), rygame.musics.end(), this), rygame.musics.end());
        UnloadMusicStream(*(rl::Music *) audio.get());
    }
    else
    {
        UnloadSound(*(rl::Sound *) audio.get());
    }
}

// TODO - param to allow repetitions
void rg::mixer::Sound::Play() const
{
    if (isMusic)
    {
        PlayMusicStream(*(rl::Music *) audio.get());
    }
    else
    {
        PlaySound(*(rl::Sound *) audio.get());
    }
}

void rg::mixer::Sound::Stop() const
{
    if (isMusic)
    {
        if (IsMusicStreamPlaying(*(rl::Music *) audio.get()))
        {
            StopMusicStream(*(rl::Music *) audio.get());
        }
    }
    else
    {

        if (IsSoundPlaying(*(rl::Sound *) audio.get()))
        {
            StopSound(*(rl::Sound *) audio.get());
        }
    }
}

void rg::mixer::Sound::SetVolume(const float value) const
{
    if (isMusic)
    {
        SetMusicVolume(*(rl::Music *) audio.get(), value);
    }
    else
    {
        SetSoundVolume(*(rl::Sound *) audio.get(), value);
    }
}

const char *rg::mixer::Sound::GetFilename() const
{
    return file;
}
