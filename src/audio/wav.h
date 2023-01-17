#ifndef _WAV_H_
#define _WAV_H_

#include <stdint.h>

typedef struct _FMTSubchunk {
	char fmt_str[4];
	uint32_t subchunk1_size;
	uint16_t audio_format; //should be an enum (TODO)
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
} FMTSubchunk;

typedef struct _WavFile {
	void* samples;
	int num_samples;
	FMTSubchunk subchunk;
} WavFile;

WavFile* WavFile_Open(char* file_name);
void WavFile_Write(WavFile* wav, char* file_name);
void WavFile_AddSamples(WavFile* wav, int index, void* samples, int sample_num);
void WavFile_Free(WavFile* wav);

#endif //_WAV_H_