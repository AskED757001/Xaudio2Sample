#include <iostream>
#include "Xaudio.h"
#include "Generate.h"

SoundData Music;//最初から最後まで持ち歩くのでグローバル変数でお願いします

int main()
{
	std::cout << "InitXaudio2\n";
	InitXaudio2();

	std::cout << "WavLoad\n";
	WavLoad("E://Far East Allnight.wav", &Music);

	std::cout << "WavLoadFromHeader\n";
	WavLoadFromHeader(&Music);

	std::cout << "SetBuffer\n";
	SetBuffer(&Music, false);

	std::cout << "SoundControl\n";
	SoundControl(1, 0.1, &Music);

	std::cout << "If you want to stop sound, please push enter.\n";
	getchar();

	std::cout << "SoundStop\n";
	SoundStop(&Music);

	std::cout << "DisposeBuffer\n";
	DisposeBuffer(&Music);

	std::cout << "DisposeSource\n";
	DisposeSource(&Music);

	std::cout << "DisposeXaudio2\n";
	DisposeXaudio2();

	std::cout << "End of program.";
	return 0;
}