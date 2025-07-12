#include <iostream>
#include <cmath>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <portaudio.h>
#include <vector>
#include <algorithm>
#include <map>

const int SAMPLE_RATE = 44100;

typedef struct {
    float phase;
    float amplitude;
    bool  is_playing;
    float frequency;
    float time;
    float volume;
    float d_volume;
}
note;

typedef struct {
    std::vector<note> notes;
    float amplitude;
    std::map<int, int> key_to_note;
    int waveform;
    float dx;
}
pa_data;


static int call_back(const void *input_buffer, void *output_buffer,
                     unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info,
                     PaStreamCallbackFlags status_flags, void *synth_data);

void play(const char* device_path);

int get_note(pa_data *data);

float generate_waveform(float phase, int waveform) {
    float normalizedPhase = phase / (2.0f * M_PI);
    switch (waveform) {
        case 0: // sine
            return sin(phase);
        case 1: // sawthooth
            return (normalizedPhase * 2.0f) - 1.0f;
        case 2: { // square
            float value = 0.0f;
            for (int k = 1; k <= 7; k += 2) {
                value += sin(k * phase) / k;
            }
            return (4.0f / M_PI) * value;
        }
        case 3:
            return 2.0f * fabs(2.0f * normalizedPhase - 1.0f) - 1.0f;
        default:
            return sin(phase);
    }
}

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
        float sound = 0.0f;
        int active_notes = 0;

        for (size_t j = 0; j < data->notes.size(); j++) {
            if (data->notes[j].is_playing) {
                data->notes[j].volume += data->notes[j].d_volume * data->dx;

                if (data->notes[j].volume < 1.0f) {
                    data->notes[j].d_volume += (1.0f - data->notes[j].volume) * data->dx;
                } else {
                    data->notes[j].d_volume -= 0.01f * data->dx;
                }

                data->notes[j].volume = std::max(0.0f, std::min(data->notes[j].volume, 1.0f));

                float waveform = generate_waveform(data->notes[j].phase, data->waveform);

                sound += (waveform * data->notes[j].volume);
                active_notes++;

                data->notes[j].time += data->dx;
            }

            data->notes[j].phase += (data->notes[j].frequency * 2.0f * M_PI) / SAMPLE_RATE;
            if (data->notes[j].phase >= 2.0f * M_PI)
                data->notes[j].phase -= 2.0f * M_PI;
        }

        if (active_notes > 0) {
            sound = sound * data->amplitude / active_notes;
        }

        *out++ = sound;
    }
    return paContinue;
}

int get_note(pa_data *data) {
    for (size_t i = 0; i < data->notes.size(); i++) {
        if (!data->notes[i].is_playing) {
            return i;
        }
    }

   data->notes.push_back({0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 3.0f});
   return data->notes.size() - 1;
}

void play(const char* device_path) {
    PaError err;
    PaStream *stream;
    pa_data data;
    data.amplitude  = 0.5f;
    data.waveform   = 2;
    data.dx         = 1.0f / SAMPLE_RATE;

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

    std::map<int, float> key_frequencies = {
        {KEY_A, 130.81f}, // C4
        {KEY_S, 138.59f}, // C#4
        {KEY_D, 146.83f}, // D4
        {KEY_F, 155.56f}, // D#4
        {KEY_G, 164.81f}, // E4
        {KEY_H, 174.61f}, // F4
        {KEY_J, 185.0f},  // F#4
        {KEY_K, 196.0f},  // G4
        {KEY_K, 207.65f}, // G#4
        {KEY_L, 220.0f},  // A
        {KEY_SEMICOLON, 233.08f},   // A#4
        {KEY_APOSTROPHE, 246.94f},  // B4
        {KEY_Q, 261.63f}, // C
        {KEY_2, 277.18f}, // C#
        {KEY_W, 293.66f}, // D
        {KEY_3, 311.13f}, // D#
        {KEY_E, 329.63f}, // E
        {KEY_R, 349.23f}, // F
        {KEY_5, 369.99f}, // F#
        {KEY_T, 392.0f},  // G
        {KEY_6, 415.30f}, // G#
        {KEY_Y, 440.0f},  // A
        {KEY_7, 466.16f}, // A#
        {KEY_U, 493.88f}, // B
        {KEY_I, 523.25f}, // C6
        {KEY_9, 554.37f}, // C#6
        {KEY_O, 587.33f}, // D6
        {KEY_0, 622.25f}, // D#6
        {KEY_P, 659.25f}, // E6
        {KEY_LEFTBRACE, 698.46f},  // F6
        {KEY_EQUAL, 739.99f},      // F#6
        {KEY_RIGHTBRACE, 783.99f}  // G
    };

    input_event event;
    bool playing = true;

    while (playing) {
        ssize_t n = read(fd, &event, sizeof(event));
        if (n == sizeof(event)) {
            if (event.type == EV_KEY) {
                auto freq_it = key_frequencies.find(event.code);
                if (freq_it != key_frequencies.end()) {
                    if (event.value == 1) {
                        int note_idx = get_note(&data);
                        data.notes[note_idx].time = 0.0f;
                        data.notes[note_idx].frequency = freq_it->second;
                        data.notes[note_idx].is_playing = true;
                        data.key_to_note[event.code] = note_idx;
                    } else if (event.value == 0) {
                        auto note_it = data.key_to_note.find(event.code);
                        if (note_it != data.key_to_note.end()) {
                            data.notes[note_it->second].is_playing = false;
                            data.key_to_note.erase(note_it);
                        }
                    }
                } else if (event.code == KEY_Z && event.value == 1) {
                    playing = false;
                }
                else if (event.code == KEY_X) data.waveform = 0;
                else if (event.code == KEY_C) data.waveform = 1;
                else if (event.code == KEY_V) data.waveform = 2;
                else if (event.code == KEY_B) data.waveform = 3;

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
