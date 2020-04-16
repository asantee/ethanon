#ifndef GS2D_FM_AUDIO_CONTEXT_H_
#define GS2D_FM_AUDIO_CONTEXT_H_

#include "../../Audio.h"
#include "../../Platform/FileLogger.h"

#include <fmod.hpp>

#include <boost/weak_ptr.hpp>

namespace gs2d {

bool FMOD_ERRCHECK_fn(FMOD_RESULT result, const char *file, int line, Platform::FileLogger& logger);
#define FMOD_ERRCHECK(_result, _logger) FMOD_ERRCHECK_fn(_result, __FILE__, __LINE__, _logger)

class FMAudioContext : public Audio
{
	static FMOD::System* m_system;

	float m_globalVolume;

	bool CreateAudioDevice(boost::any data) override;
	Platform::FileLogger m_logger;

	boost::weak_ptr<FMAudioContext> weak_this;

	FMAudioContext();

public:
	~FMAudioContext();

	static boost::shared_ptr<FMAudioContext> Create(boost::any data);

	static bool IsStreamable(const Audio::SAMPLE_TYPE type);

	static void CommonInit(Platform::FileLogger& logger);

	AudioSamplePtr LoadSampleFromFile(
		const std::string& fileName,
		const Platform::FileManagerPtr& fileManager,
		const Audio::SAMPLE_TYPE type = Audio::UNKNOWN_TYPE) override;

	AudioSamplePtr LoadSampleFromFileInMemory(
		void *pBuffer,
		const unsigned int bufferLength,
		const Audio::SAMPLE_TYPE type = Audio::UNKNOWN_TYPE) override;

	boost::any GetAudioContext() override;

	void SetGlobalVolume(const float volume) override;
	float GetGlobalVolume() const override;

	void Update() override;
};

typedef boost::shared_ptr<FMAudioContext> FMAudioContextPtr;
typedef boost::shared_ptr<FMAudioContext> FMAudioContextWeakPtr;

} // namespace gs2d

#endif
