
#ifndef WIN32
#include <string.h>
#endif
#include <stdio.h>
#include <math.h>

#include <IHandleSys.h>

#define TAGLIB_STATIC
#include <fileref.h>
#include <tag.h>

#include "wave.h"

#define SOUNDTYPE_WAVE 0
#define SOUNDTYPE_MP3 1


HandleType_t g_SoundFileType;




class SoundFile {

public:
	SoundFile(char *path) {

		file = NULL;
		tag = NULL;

		char *file_extension = strrchr(path, '.');

		if (file_extension == NULL) {
			return;
		}

		if (strcmp(file_extension, ".wav") == 0) {
			type = SOUNDTYPE_WAVE;
		}
		else if (strcmp(file_extension, ".mp3") == 0) {
			type = SOUNDTYPE_MP3;
		}
		else {
			return;
		}

		file = new TagLib::FileRef(path);

		loadTag();
	}

	~SoundFile() {
		close();
	}
	
	bool isOpen() {

		if (file == NULL) {
			return false;
		}

		if (file->isNull()) {
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

		// Fix for TagLib not returning Wav length correctly
		if (type == SOUNDTYPE_WAVE) {

			WavFileForIO *wave = new WavFileForIO();
			TagLib::FileName filePath = file->file()->name();
			const char *path = filePath;
			wave->setPath((char *)path);

			if (!wave->read()) {
				delete wave;

				return -1;
			}

			float duration = wave->getSongDuration();

			delete wave;

			return (size_t)ceil(duration);
		}

		size_t test = properties->length();

		return properties->length();
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

	TagLib::FileRef* file;
	TagLib::Tag* tag;
	size_t type;
};