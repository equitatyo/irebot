#include "wav.h"

#include "includes.h"

#include <sys/stat.h>

//Opens a WAV file
//Returns a null pointer if an error occurs
WavFile* WavFile_Open(char* file_name) {
	WavFile* wav = malloc(sizeof(WavFile));
	memset(wav, 0, sizeof(WavFile));

	FILE* f = fopen(file_name, "rb+");

	if (!f) {
		printf("%s\n", file_name);
		printf("Error while opening file\n");
		return NULL;
	}

	struct stat file_info;
	stat(file_name, &file_info);

	unsigned char* contents = malloc(file_info.st_size);
	fread(contents, file_info.st_size, 1, f);

	fclose(f);

	if (file_info.st_size < 5) {
		printf("File too small\n");
		return NULL;
	}

	char riff_tag[5];
	memcpy((void*)riff_tag, (const void*)contents, 4);
	riff_tag[4] = '\0';

	if (strcmp(riff_tag, "RIFF")) {
		printf("Invalid WAV file (no RIFF tag)\n");
		return NULL;
	}

	FMTSubchunk subchunk;
	memcpy((void*)&subchunk, (const void*)(contents + 0xC), sizeof(FMTSubchunk));

	if (subchunk.bits_per_sample == 24) {
		printf("Unsupported bits per sample (24)\n");
		return NULL;
	}

	int bytes_per_sample = subchunk.bits_per_sample / 8;

	if (bytes_per_sample > sizeof(int)) {
		printf("Bits per sample larger than default int (%i)\n", sizeof(int));
		return NULL;
	}

	unsigned char* data = contents + 0xC + sizeof(FMTSubchunk) + 0x8;

	uint32_t size_of_data;
	memcpy((void*)&size_of_data, (const void*)(data - 0x4), 4);

	printf("data size: %i\n", size_of_data);
	printf("real data size: %i\n", _msize(contents) - (0xC + sizeof(FMTSubchunk) + 0x8));

	void* samples = malloc(size_of_data);
	int num_samples = size_of_data / subchunk.num_channels / bytes_per_sample;

	for (int i = 0; i < num_samples; i += subchunk.num_channels) {
		void* _samples = malloc(bytes_per_sample * subchunk.num_channels);
		
		for (int j = 0; j < subchunk.num_channels; j++) {
			memcpy(_samples + j * bytes_per_sample, (const void*)(data + (i + j) * bytes_per_sample), bytes_per_sample);
		}

		memcpy(samples + i, _samples, bytes_per_sample * subchunk.num_channels);

		//if (_samples) free(_samples); this crashes for unknown reasons
	}

	free(contents);

	wav->samples = samples;
	wav->num_samples = num_samples;
	wav->subchunk = subchunk;
	return wav;
}

void WavFile_AddSamples(WavFile* wav, int index, void* samples, int sample_num) {
	int bytes_per_sample = wav->subchunk.bits_per_sample / 8;
	if (index + sample_num > wav->num_samples) { printf("wat\n"); return; }
	for (int i = 0; i < sample_num; i++) { //do not hardcode the type in the future
		*((int32_t*)(wav->samples + (index * bytes_per_sample * wav->subchunk.num_channels))) += *((int32_t*)(samples + (i * bytes_per_sample * wav->subchunk.num_channels)));
	}
}

void WavFile_Write(WavFile* wav, char* file_name) {
	FILE* f = fopen(file_name, "wb+");

	fwrite((void*)"RIFF", 4, 1, f);

	char stuff[4] = { 0x8c, 0x2a, 0x00, 0x00 };

	fwrite((void*)stuff, 4, 1, f);
	fwrite((void*)"WAVE", 4, 1, f);

	fwrite((void*)&wav->subchunk, sizeof(FMTSubchunk), 1, f);

	fwrite((void*)"data", 4, 1, f);

	int bytes_per_sample = wav->subchunk.bits_per_sample / 8;
	uint32_t data_size = wav->num_samples * wav->subchunk.num_channels * bytes_per_sample;

	fwrite((void*)&data_size, 4, 1, f);	

	void* samples = wav->samples;

	printf("_msize(wav->samples) =  %i\n", _msize(wav->samples));
	printf("wav->num_samples = %i\n", wav->num_samples);
	printf("data_size = %i\n", data_size);

	for (int i = 0; i + (wav->subchunk.num_channels / 2) < wav->num_samples; i += wav->subchunk.num_channels) {
		for (int j = 0; j < wav->subchunk.num_channels; j++) {
			fwrite(samples + (i * bytes_per_sample), bytes_per_sample, 1, f);
		}
		printf("%i/%i\n", i, data_size);
	}

	fclose(f);

	free(wav->samples);
	free(wav);
}

void WavFile_Free(WavFile* wav) {
	free(wav->samples);
	free(wav);
}