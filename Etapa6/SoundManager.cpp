#include "SoundManager.h"

ALuint SoundManager::sons[NSONS];
ALCcontext* SoundManager::context;
ALCdevice* SoundManager::device;

//Carrega un so, assignant-lo a un "So" (enum) concret
bool SoundManager::loadSound(const char* path, So nom) {

    int sonum = static_cast<int>(nom); //Obtenim la posició de l'array

    int channel, sampleRate, bps, size;
    char* data = loadWAV(path, channel, sampleRate, bps, size); //Carregam el WAV del so

    unsigned int bufferid, format;
    alGenBuffers(1, &bufferid); //Generam el buffer on es guardara el so
    if (channel == 1) //És mono
    {
        if (bps == 8)
        {
            format = AL_FORMAT_MONO8;
        }
        else {
            format = AL_FORMAT_MONO16;
        }
    }
    else { //És estèreo
        if (bps == 8)
        {
            format = AL_FORMAT_STEREO8;
        }
        else {
            format = AL_FORMAT_STEREO16;
        }
    }
    alBufferData(bufferid, format, data, size, sampleRate); //Omplim el buffer amb les dades del WAV

    alGenSources(1, &sons[sonum]); //Generam la font de so
    alSourcei(sons[sonum], AL_BUFFER, bufferid); //Hi ficam les dades del buffer creat

    alDeleteBuffers(1, &bufferid); //Eliminam el buffer
    delete[] data; //I les dades del WAV
    return true;
}

void SoundManager::playSound(So nom, Vector3 pos, bool forceStart) {
    int sonum = static_cast<int>(nom);

    alSource3f(sons[sonum], AL_POSITION, pos.x, pos.y, pos.z); //Establim la posició del so
    alSourcef(sons[sonum], AL_REFERENCE_DISTANCE, 5.0f); //Atenuació, a una distància de 5 ja s'haurà reduit el so a la meitat
    alSourcei(sons[sonum], AL_MAX_GAIN, 1);
    alSourcef(sons[sonum], AL_ROLLOFF_FACTOR, 1);

    ALenum estat;
    alGetSourcei(sons[sonum], AL_SOURCE_STATE, &estat);
    if (estat != AL_PLAYING || forceStart) { //Si el so ja està sonant, només el reiniciam si forceStart == true
        alSourcePlay(sons[sonum]);
    }
}

//Inicialització
bool SoundManager::initialize() {

    device = alcOpenDevice(nullptr);
    if (device == NULL) //No s'ha trobat dispositiu
    {
        printf("NO DEVICE");
        return false;
    }

    //Model d'atenuació
    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    //Cream el contexte
    context = alcCreateContext(device, nullptr);

    //I el posam com a actiu
    alcMakeContextCurrent(context);

    return true;
}