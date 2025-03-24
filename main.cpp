#include <iostream>
#include "portaudio.h"
#include <cmath>
const int SAMPLE_RATE = 44100;

typedef struct {
    float phase;
    float amplitude;
    float frequency;
    bool  is_playing;
}
pa_data;

static int call_back(const void *input_buffer, void *output_buffer, unsigned long frames_per_buffer,
                     const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
                     void *synth_data) {
    pa_data *data = (pa_data*)synth_data;
    float *out = (float*)output_buffer;
    (void) input_buffer;

    for (unsigned long i = 0; i < frames_per_buffer; i++) {
        if (data->is_playing) {
            float value = data->amplitude * sin(data->phase);
            *out++ = value;
            data->phase += (data->frequency * 2.0f * M_PI) / SAMPLE_RATE;
            if (data->phase >= 2.0f * M_PI)
                data->phase -= 2.0f * M_PI;
        } else {
            *out++ = 0.0f;
        }
    }
    return paContinue;
}

int main() {
    PaError err;
    PaStream *stream;
    pa_data data;
    data.amplitude  = 0.5f;
    data.is_playing = false;
    data.phase      = 0.0f;
    data.frequency  = 440.0f;

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, 44100, 512, call_back, &data);
    if (err != paNoError) {
        std::cerr << "Error opening PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Error starting PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }
    bool is_playing = true;
    char key = 0;

    while (is_playing) {
        std::cin >> key;

        switch (key) {
            case 'q':
                data.frequency = 261.63f;
                data.is_playing = true;
                break;
            case '2':
                data.frequency = 277.18f;
                data.is_playing = true;
                break;
            case 'w':
                data.frequency = 293.66f;
                data.is_playing = true;
                break;
            case '3':
                data.frequency = 311.13f;
                data.is_playing = true;
                break;
            case 'e':
                data.frequency = 329.63f;
                data.is_playing = true;
                break;
            case 'r':
                data.frequency = 349.23f;
                data.is_playing = true;
                break;
            case '5':
                data.frequency = 369.99f;
                data.is_playing = true;
                break;
            case 't':
                data.frequency = 392.0f;
                data.is_playing = true;
                break;
            case '6':
                data.frequency = 415.3f;
                data.is_playing = true;
                break;
            case 'y':
                data.frequency = 440.0f;
                data.is_playing = true;
                break;
            case '7':
                data.frequency = 466.16f;
                data.is_playing = true;
                break;
            case 'u':
                data.frequency = 493.88f;
                data.is_playing = true;
                break;
            case '0':
                data.is_playing = false;
                break;
            case 'z':
                return 0;
            default:
                break;
        }
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "Error stopping PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "Error closing PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_Terminate();
    if (err != paNoError) {
        std::cerr << "Error terminating PortAudio: " << Pa_GetErrorText(err) << std::endl;
    }
}
