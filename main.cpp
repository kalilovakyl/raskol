#include <iostream>
#include <cmath>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <portaudio.h>
const int SAMPLE_RATE = 44100;

typedef struct {
    float phase;
    float amplitude;
    float frequency;
    bool  is_playing;
}
pa_data;


static int call_back(const void *input_buffer, void *output_buffer,
                     unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info,
                     PaStreamCallbackFlags status_flags, void *synth_data);

void play(const char* device_path);

int main() {
    play("/dev/input/event3");
}

static int call_back(const void *input_buffer, void *output_buffer, unsigned long frames_per_buffer,
                     const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
                     void *synth_data) {
    pa_data *data = (pa_data*)synth_data;
    float *out = (float*)output_buffer;
    (void) input_buffer;


    for (unsigned long i = 0; i < frames_per_buffer; i++) {
        if (data->is_playing) {
            float value = data->amplitude * sin(data->phase);
            float square = 0.0f;
            if (value > 0) square = 1.0f;
            else square = -1.0f;
            float saw = (data->phase / (M_PI)) - 1.0f;
            *out++ = saw;
            data->phase += (data->frequency * 2.0f * M_PI) / SAMPLE_RATE;
            if (data->phase >= 2.0f * M_PI)
                data->phase -= 2.0f * M_PI;
        } else {
            *out++ = 0.0f;
        }
    }
    return paContinue;
}

void play(const char* device_path) {
    PaError err;
    PaStream *stream;
    pa_data data;
    data.amplitude  = 0.5f;
    data.is_playing = false;
    data.phase      = 0.0f;
    data.frequency  = 440.0f;

    int fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error opening device: " << device_path << std::endl;
        return;
    }

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization error: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, 44100, 512, call_back, &data);
    if (err != paNoError) {
        std::cerr << "Error opening PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Error starting PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return;
    }

    input_event event;
    bool playing = true;

    while (playing) {
        ssize_t n = read(fd, &event, sizeof(event));
        if (n == sizeof(event)) {
            if (event.type == EV_KEY) {
                if (event.code == KEY_Q) {
                    if (event.value == 1) {
                        data.frequency  = 523.25f; // C
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_2) {
                    if (event.value == 1) {
                        data.frequency  = 277.18;  // C#
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_W) {
                    if (event.value == 1) {
                        data.frequency  = 293.66f; // D
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_3) {
                    if (event.value == 1) {
                        data.frequency  = 311.13f;  // D#
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_E) {
                    if (event.value == 1) {
                        data.frequency  = 329.63f; // E
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_R) {
                    if (event.value == 1) {
                        data.frequency  = 349.23f; // F
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_5) {
                    if (event.value == 1) {
                        data.frequency  = 369.99f; // F#
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_T) {
                    if (event.value == 1) {
                        data.frequency  = 392.0f; // G
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_6) {
                    if (event.value == 1) {
                        data.frequency  = 415.3f; // G#
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_Y) {
                    if (event.value == 1) {
                        data.frequency  = 440.0f; // A
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_7) {
                    if (event.value == 1) {
                        data.frequency  = 466.16f; // A#
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_U) {
                    if (event.value == 1) {
                        data.frequency  = 493.88f; // B
                        data.is_playing = true;
                    } else if (event.value == 0) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_0) {
                    if (event.value == 1) {
                        data.is_playing = false;
                    }
                }
                if (event.code == KEY_Z) {
                    if (event.value == 1) {
                        playing = false;
                    }
                }
            }
        }


    }
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "Error stopping PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "Error closing PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    err = Pa_Terminate();
    if (err != paNoError) {
        std::cerr << "Error terminating PortAudio: " << Pa_GetErrorText(err) << std::endl;
    }

    close(fd);
}