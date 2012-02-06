
#ifndef WIN32
#include <string.h>
#endif
#include <stdio.h>
#include <math.h>

#include <IHandleSys.h>

#define TAGLIB_STATIC
#include <fileref.h>
#include <tag.h>
#include "mpeg/mpegfile.h"
#include "mpeg/xingheader.h"
#include "riff/wav/wavfile.h"

#include <tbytevector.h>

#define SOUNDTYPE_WAVE 0
#define SOUNDTYPE_MP3 1


HandleType_t g_SoundFileType;


// Because TagLib is hiding members from us we have to trick a little bit...
class SoundLib_WavFile : public TagLib::RIFF::WAV::File {

public:
	float getSoundLength() {
		unsigned int streamLength = 0;

		for (TagLib::uint i = 0; i < chunkCount(); i++) {
			if (chunkName(i) == "data") {
				streamLength = chunkDataSize(i);
			}
		}

		float byteRate = float(audioProperties()->bitrate()) * 125.0f;

		return byteRate > 0.0f ? float(streamLength) / byteRate : 0.0f;
	}
};

class SoundFile {

private:
	TagLib::File* file;
	TagLib::Tag* tag;
	size_t type;

public:
	SoundFile(char *path) {

		file = NULL;
		tag = NULL;

		char *file_extension = strrchr(path, '.');

		if (file_extension == NULL) {
			return;
		}

		if (strcmp(file_extension, ".wav") == 0) {
			file = new TagLib::RIFF::WAV::File(path);
			type = SOUNDTYPE_WAVE;
		}
		else if (strcmp(file_extension, ".mp3") == 0) {
			file = new TagLib::MPEG::File(path);
			type = SOUNDTYPE_MP3;
		}
		else {
			return;
		}

		loadTag();
	}

	~SoundFile() {
		close();
	}
	
	bool isOpen() {

		if (file == NULL) {
			return false;
		}

		if (!file->isValid()) {
			return false;
		}
		
		return true;
	}

	bool loadTag() {

		if (tag == NULL) {
			tag = file->tag();
			return true;
		}

		return false;
	}

	size_t getSoundDuration() {

		TagLib::AudioProperties *properties = file->audioProperties();

		if (!properties) {
			return -1;
		}

		if (type == SOUNDTYPE_WAVE) {
			SoundLib_WavFile* f = (SoundLib_WavFile*)file;
			return f->audioProperties()->length();
		}
		else {
			TagLib::MPEG::File* f = (TagLib::MPEG::File*)file;
			return f->audioProperties()->length();
		}

		return 0;
	}

	float getSoundDurationFloat() {
		
		TagLib::AudioProperties *properties = file->audioProperties();
		
		if (!properties) {
			return -1;
		}

		if (type == SOUNDTYPE_WAVE) {
			SoundLib_WavFile* f = (SoundLib_WavFile*)file;
			return f->getSoundLength();
		}
		else {
			TagLib::MPEG::File* f = (TagLib::MPEG::File*)file;

			long first = f->firstFrameOffset();

			f->seek(first);
			TagLib::MPEG::Header firstHeader(f->readBlock(4));
			int xingHeaderOffset = TagLib::MPEG::XingHeader::xingHeaderOffset(firstHeader.version(), firstHeader.channelMode());

			f->seek(first + xingHeaderOffset);
			TagLib::MPEG::XingHeader xingHeader(f->readBlock(16));

			// Read the length and the bitrate from the Xing header.
			if (xingHeader.isValid() && firstHeader.sampleRate() > 0 && xingHeader.totalFrames() > 0) {

				double timePerFrame = double(firstHeader.samplesPerFrame()) / firstHeader.sampleRate();

				return float(timePerFrame * xingHeader.totalFrames());
			}
			else {
				float byteRate = (float)f->audioProperties()->bitrate() * 125.0f; // 1000 / 8 = 125 (optimization)
				float length = (float)(f->length() - f->firstFrameOffset()) / byteRate;

				return length;
			}
		}

		return 0.0;
	}

	size_t getSoundBitRate() {
		
		TagLib::AudioProperties *properties = file->audioProperties();

		if (!properties) {
			return -1;
		}

		return properties->bitrate();
	}

	size_t getSoundSamplingRate() {
		
		TagLib::AudioProperties *properties = file->audioProperties();

		if (!properties) {
			return -1;
		}

		return properties->sampleRate();
	}

	void getSoundArtist(char *buf, size_t size) {

		if (!tag) {
			buf[0] = '\0';
			return;
		}

		TagLib::String tl_str = tag->artist();
		const char *str = tl_str.toCString(true);
		strncpy(buf, str, size);

		return;
	}

	void getSoundTitle(char *buf, size_t size) {

		if (!tag) {
			buf[0] = '\0';
			return;
		}

		TagLib::String tl_str = tag->title();
		const char *str = tl_str.toCString(true);
		strncpy(buf, str, size);

		return;
	}

	size_t getSoundNum() {

		if (!tag) {
			return -1;
		}

		return tag->track();
	}

	void getSoundAlbum(char *buf, size_t size) {

		if (!tag) {
			buf[0] = '\0';
			return;
		}

		TagLib::String tl_str = tag->album();
		const char *str = tl_str.toCString(true);
		strncpy(buf, str, size);

		return;
	}

	size_t getSoundYear() {

		if (!tag) {
			return -1;
		}

		return tag->year();
	}

	void getSoundComment(char *buf, size_t size) {

		if (!tag) {
			buf[0] = '\0';
			return;
		}

		TagLib::String tl_str = tag->comment();
		const char *str = tl_str.toCString(true);
		strncpy(buf, str, size);

		return;
	}

	void getSoundGenre(char *buf, size_t size) {

		if (!tag) {
			buf[0] = '\0';
			return;
		}

		TagLib::String tl_str = tag->genre();
		const char *str = tl_str.toCString(true);
		strncpy(buf, str, size);

		return;
	}


private:

	void close() {

		delete file;

		return;
	}
};