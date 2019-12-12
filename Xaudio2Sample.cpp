#include <iostream>
#include "Xaudio.h"

SoundData Music;//最初から最後まで持ち歩くのでグローバル変数でお願いします

int main()
{
	std::cout << "InitXaudio2\n";
	InitXaudio2();
	std::cout << "WavLoad\n";
	WavLoad("E://Run0.wav", &Music);
	std::cout << "WavLoadFromHeader\n";
	WavLoadFromHeader(&Music, false);
	std::cout << "SoundControl\n";
	SoundControl(1, 1, &Music);
	std::cout << "If you want to stop sound, please push enter.\n";
	getchar();
	std::cout << "SoundStop\n";
	SoundStop(&Music);
	std::cout << "DisposeBuffer\n";
	DisposeBuffer(&Music);
	std::cout << "DisposeXaudio2\n";
	DisposeXaudio2();
	std::cout << "End of program.";
	return 0;
}