#include "clicks.h"

#include "includes.h"
#include <sys/stat.h>
#include <time.h>

int folder_exists(char* folder) {
    struct stat sb;

    return stat(folder, &sb) == 0 && S_ISDIR(sb.st_mode);
}

int get_index_from_filename(char* filename) {
    int num;
    sscanf(filename, "%d.wav", &num);
    return num;
}

drwav files[32];
Clicks get_clicks_in_folder(char* path) {
    memset(files, 0, sizeof(drwav) * 32);
    WIN32_FIND_DATA file;
    HANDLE handle = NULL;

    char filter[2048];

    sprintf(filter, "%s\\*.*", path);

    handle = FindFirstFile(filter, &file);

    short unsigned count = 0;
    short signed start = -1;

    do
    {
        if(strcmp(file.cFileName, ".") != 0 && strcmp(file.cFileName, "..") != 0)
        {
            if(!(file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                char full_path[256];
                sprintf_s(full_path, 256, "%s/%s", path, file.cFileName);
                int index = get_index_from_filename(file.cFileName);
                drwav_init_file(&files[index], full_path, NULL);

                if (start < 0) start = index;
                count++;
                if (count > 32) {
                    printf("Too many clicks (>32)");
                    break;
                }
            }
        }
    }
    while(FindNextFile(handle, &file));

    FindClose(handle);

    Clicks clicks = {0};
    clicks.count = count;
    clicks.files = malloc(32 * sizeof(drwav));
    memcpy(clicks.files, files, 32 * sizeof(drwav));
    clicks.start = start;

    return clicks;
}

// [0] -> Clicks
//      [0] -> clicks
//          [0] drwav struct of clicks/1.wav
//          [1] drwav struct of clicks/2.wav
//          ...
//      [1] -> softClicks
//          [0] drwav struct of softClicks/1.wav
//          ...
//[1] -> Releases
//      [0] -> releases
//          [0] drwav struct of releases/1.wav
//          ...
//      [1] -> softReleases
//          [0] drwav struct of softReleases/1.wav
//          ...
Clicks** load_clicks(char* clicks_path) {
    Clicks** samples = malloc(2 * sizeof(uintptr_t));
    samples[0] = malloc(2 * sizeof(Clicks));
    samples[1] = malloc(2 * sizeof(Clicks));

    char* paths[4] = {"clicks", "softClicks", "releases", "softReleases"};

    for (int i = 0; i < 4; i++) {
        char path[256];
        sprintf_s(path, 256, "%s\\%s", clicks_path, paths[i]);
        Clicks clicks = get_clicks_in_folder(path);

        if (i >= 2)
            samples[1][i % 2] = clicks;
        else
            samples[0][i % 2] = clicks;
    }

    return samples;
}

void unload_clicks(Clicks** files) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = files[i][j].start; k < files[i][j].count; k++) {
                drwav_uninit(&files[i][j].files[k]);
            }
            free(files[i][j].files);
        }
    }
}

float frames_to_samples(float frames, float fps, float sample_rate) {
    return (frames / fps) * sample_rate;
}

void write_samples(drwav in, int32_t* out, int offset, float volume) {
    size_t size = (size_t)(in.totalPCMFrameCount * in.channels);
    int32_t samples[size];

    int r = drwav_read_pcm_frames_s32(&in, in.totalPCMFrameCount, samples);

    for (int i = 0; i < size; i += in.channels)
        for (int j = 0; j < in.channels; j++)
            out[offset + i + j] += samples[i + j] * volume;
}

int rand_range(int min, int max) {
    return rand() % (max - min) + min;
}

drwav get_random_click(Clicks clicks) {
    return clicks.files[clicks.start];
    drwav click;
    do {
        int rand_index = rand_range(clicks.start, clicks.count + clicks.start);
        click = clicks.files[rand_index];

    } while (!click.sampleRate);
    return click;
}

int generate_clicks(char* path, Macro* macro, Clicks** player1, Clicks** player2, float lsoft_clicks, float nsoft_clicks, float volume1, float volume2) {
    drwav sample_click = player1[0][0].files[player1[0][0].start];

    long unsigned sample_len = frames_to_samples((float)macro->actions[macro->action_num-1].frame, (float)macro->fps, (float)sample_click.sampleRate) + sample_click.totalPCMFrameCount * 2;

    int32_t* samples = malloc(sample_len * sample_click.channels * sizeof(int32_t));

    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = sample_click.channels;
    format.sampleRate = sample_click.sampleRate;
    format.bitsPerSample = sample_click.bitsPerSample;

    drwav output;
    drwav_init_file_write(&output, path, &format, NULL);

    time_t epoch_seconds = time(NULL);
    srand(epoch_seconds);

    int p1_has_held = false;
    int p2_has_held = false;

    int p1_was_hold_soft = false;
    int p2_was_hold_soft = false;

    for (int i = 0; i < macro->action_num; i++) {
        Action action = macro->actions[i];

        if (action.player1_holding) {
            int is_soft = false;
            int next_delta = -1;
            int last_delta = -1;

            if (i + 1 < macro->action_num) {
                next_delta = macro->actions[i + 1].frame - action.frame;
            }

            if (i > 0) {
                last_delta = action.frame - macro->actions[i - 1].frame;
            }

            //possibly change these
            if (next_delta != -1 && next_delta <= nsoft_clicks) is_soft = true;
            if (last_delta != -1 && last_delta <= lsoft_clicks) is_soft = true;

            drwav click;
            if (is_soft && player1[0][1].count) click = get_random_click(player1[0][1]);
            else click = get_random_click(player1[0][0]);

            write_samples(click, samples, frames_to_samples((float)action.frame, (float)macro->fps, (float)output.sampleRate), volume1 / 100.f);

            p1_was_hold_soft = is_soft;
            p1_has_held = true;
        } else if (p1_has_held) {
            drwav click;
            if (p1_was_hold_soft && player1[1][1].count) click = get_random_click(player1[1][1]);
            else click =  get_random_click(player1[1][0]);

            write_samples(click, samples, frames_to_samples((float)action.frame, (float)macro->fps, (float)output.sampleRate), volume1 / 100.f);

            p1_has_held = false;
        }

        if (action.player2_holding) {
            int is_soft = false;
            int next_delta = -1;
            int last_delta = -1;

            for (int j = i + 1; !macro->actions[j].player2_holding && j < macro->action_num && j - i < 50; j++) {
                next_delta = macro->actions[j].frame - action.frame;
            }

            if (i > 0) {
                last_delta = action.frame - macro->actions[i - 1].frame;
            }

            if (next_delta != -1 && next_delta <= nsoft_clicks) is_soft = true;
            if (last_delta != -1 && last_delta <= lsoft_clicks) is_soft = true;

            drwav click;
            if (is_soft && player2[0][1].count) click = get_random_click(player2[0][1]);
            else click = get_random_click(player2[0][0]);

            write_samples(click, samples, frames_to_samples((float)action.frame, (float)macro->fps, (float)output.sampleRate), volume1 / 100.f);

            p2_was_hold_soft = is_soft;
            p2_has_held = true;
        } else if (p2_has_held) {
            drwav click;
            if (p2_was_hold_soft && player2[1][1].count) click = get_random_click(player2[1][1]);
            else click =  get_random_click(player2[1][0]);

            write_samples(click, samples, frames_to_samples((float)action.frame, (float)macro->fps, (float)output.sampleRate), volume1 / 100.f);

            p2_has_held = false;
        }
    }

    printf("Writing to %s\n", path);
    drwav_write_pcm_frames(&output, sample_len, samples);

    drwav_uninit(&output);

    printf("Done\n");
}
