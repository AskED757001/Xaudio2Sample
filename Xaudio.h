#include <windows.h>
#include <string>
#include <xaudio2.h>
IXAudio2* xaudio;
IXAudio2MasteringVoice* mastering_voice;
FILE* fp;

struct SoundData
{
	int bitParSample;
	int channelCnt;
	int chunkSize;
	int samplingRate;
	signed short* WaveData;
	IXAudio2SourceVoice* source_voice;
};
SoundData emptydata;
void InitXaudio2()//Xaudio2������ �ŏ��ɓǂݍ���
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	UINT32 flags = 0;
#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
	XAudio2Create(&xaudio, flags);

	xaudio->CreateMasteringVoice(&mastering_voice);

	emptydata.bitParSample = 16;
	emptydata.channelCnt = 1;
	emptydata.WaveData = (short*)malloc(sizeof(signed short));
	emptydata.chunkSize = sizeof(signed short);
	emptydata.samplingRate = 44100;

}
bool WavLoad(std::string Pass, SoundData* local_data)
//string�̃t�@�C���p�X����ǂݍ���wav�t�@�C����SoundData�� 
//���g�قƂ�� http://www-fps.nifs.ac.jp/ito/memo/openal02.html
//wav�̓ǂݍ��݂Ɏ��s�����false���Ԃ���SoundData�̒��g�������f�[�^�ɂȂ�
{
	int dataSize, chunkSize, samplingRate, byteParSec, dataPos, res32;
	short res16, channelCnt, bitParSample, blockSize;
	int flag = 0;
	fopen_s(&fp, Pass.c_str(), "rb");
	if (fp != 0)
	{
		fread(&res32, 4, 1, fp);
		if (res32 != 0x46464952)
		{
			*local_data = emptydata;
			return false;
		}
		fread(&dataSize, 4, 1, fp);
		fread(&res32, 4, 1, fp);
		if (res32 != 0x45564157)
		{
			*local_data = emptydata;
			return false;
		}
		fread(&res32, 4, 1, fp);
		if (res32 != 0x20746d66)
		{
			*local_data = emptydata;
			return false;
		}
		fread(&chunkSize, 4, 1, fp);
		fread(&res16, 2, 1, fp);
		fread(&channelCnt, 2, 1, fp);
		fread(&samplingRate, 4, 1, fp);
		fread(&byteParSec, 4, 1, fp);
		fread(&blockSize, 2, 1, fp);
		fread(&bitParSample, 2, 1, fp);
		fread(&res32, 4, 1, fp);
		if (res32 != 0x61746164)
		{
			*local_data = emptydata;
			return false;
		}
		fread(&chunkSize, 4, 1, fp);
		dataPos = ftell(fp);

		if (dataPos != ftell(fp))
			fseek(fp, dataPos, SEEK_SET);
		local_data->WaveData = (short*)malloc(chunkSize);
		if (local_data->WaveData != NULL)
		{
			fread(local_data->WaveData, chunkSize, 1, fp);
			local_data->bitParSample = bitParSample;
			local_data->channelCnt = channelCnt;
			local_data->chunkSize = chunkSize;
			local_data->samplingRate = samplingRate;
			return true;
		}
		else
		{
			*local_data = emptydata;
			return false;
		}
		fclose(fp);
	}
	else
	{
		*local_data = emptydata;
		return false;
	}
}
void WavLoadFromHeader(SoundData* local_data)//SoundData�����ꂽ�������������Ń\�[�X��
{
	if (local_data != 0)
	{
		WAVEFORMATEX SourceVoiceformat = { 0 };
		SourceVoiceformat.wFormatTag = WAVE_FORMAT_PCM;
		SourceVoiceformat.nChannels = local_data->channelCnt;
		SourceVoiceformat.wBitsPerSample = local_data->bitParSample;
		SourceVoiceformat.nSamplesPerSec = local_data->samplingRate;
		SourceVoiceformat.nBlockAlign = SourceVoiceformat.wBitsPerSample / 8 * SourceVoiceformat.nChannels;
		SourceVoiceformat.nAvgBytesPerSec = SourceVoiceformat.nSamplesPerSec * SourceVoiceformat.nBlockAlign;
		if (local_data->source_voice == NULL)
			xaudio->CreateSourceVoice(&local_data->source_voice, &SourceVoiceformat, 0, XAUDIO2_MAX_FREQ_RATIO);
	}
}
void SetBuffer(SoundData* local_data, bool Loop)//�\�[�X�������Ńo�b�t�@��
{
	XAUDIO2_BUFFER SourceBuffer = { 0 };
	SourceBuffer.AudioBytes = local_data->chunkSize;
	SourceBuffer.pAudioData = (BYTE*)local_data->WaveData;
	SourceBuffer.Flags = XAUDIO2_END_OF_STREAM;
	if (Loop)
		SourceBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	else
		SourceBuffer.LoopCount = XAUDIO2_NO_LOOP_REGION;
	local_data->source_voice->SubmitSourceBuffer(&SourceBuffer);
}
void DisposeBuffer(SoundData* local_data)//�o�b�t�@�J��
{
	if (local_data != 0)
	{
		local_data->source_voice->FlushSourceBuffers();
	}
}
void DisposeSource(SoundData* local_data)//�\�[�X�J��
{
	if (local_data != 0)
	{
		local_data->source_voice->Stop();
		local_data->source_voice->DestroyVoice();
	}
}
void DisposeXaudio2()//Xaudio2�̊J�� ���ꂪ�Ō�
{
	if (mastering_voice != 0)
	{
		mastering_voice->DestroyVoice();
		mastering_voice = 0;
	}
	if (xaudio != 0)
	{
		xaudio->Release();
		xaudio = 0;
	}
	CoUninitialize();
}
void SoundControl(double pitch, double volume, SoundData* local_data)//�Đ����x�A���ʂ��w��(1�ŕW��)
{
	if (local_data != 0)
	{
		local_data->source_voice->Start();
		if (pitch < 0)
			pitch = 0;
		if (volume < 0)
			volume = 0;
		local_data->source_voice->SetFrequencyRatio(pitch);
		local_data->source_voice->SetVolume(volume);
	}
}
UINT64 SoundCheckPlay(SoundData* local_data)//���݈ʒu��Ԃ�
{
	if (local_data != 0)
	{
		XAUDIO2_VOICE_STATE xa2state;
		local_data->source_voice->GetState(&xa2state);
		return xa2state.SamplesPlayed;
	}
	else
		return -1;
}
void SoundStop(SoundData* local_data)//�������ꎞ��~�������Ƃ�(�ŏ�����̍Đ��ɂ̓o�b�t�@�J�����ē���Ȃ����Ȃ��Ƃ����Ȃ��炵��)
{
	if (local_data != 0)
	{
		local_data->source_voice->Stop();
	}
}