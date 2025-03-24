#include <iostream>
#include "portaudio.h"
#include <cmath>

typedef struct {
    float phase = 0.0;
}
pa_data;

static int call_back(const void *input_buffer, void *output_buffer, unsigned long frames_per_buffer,
                     const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
                     void *synth_data) {
    pa_data *data = (pa_data*)synth_data;
    float *out = (float*)output_buffer;
    (void) input_buffer;

    for (unsigned long i = 0; i < frames_per_buffer; i++) {
            float value = sin(data->phase);
            *out++ = value;
            data->phase += 0.01f;
            if (data->phase >= 1.0f)
                data->phase -= 2.0f;
    }
    return 0;
}

static pa_data data;

int main() {
    PaError err;
    PaStream *stream;

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, 44100, 256, call_back, &data);
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

    getchar();

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
