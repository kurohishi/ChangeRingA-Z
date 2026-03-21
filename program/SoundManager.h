#pragma once
#include <map>

class SoundManager
{
public:
    // サウンドデータを読み込む
    static void Load();

    // サウンドデータを消去する
    static void Release();

    // BGM を再生する
    static void PlayBGM(int sound_id);

    // 再生中の BGM を停止する
    static void StopBGM();

    // SE を再生する
    static void PlaySE(int sound_id);

private:
    // 読み込んだサウンドハンドルを管理する
    static std::map<int, int> sounds;

    // 現在再生中の BGM を保持する
    static int currentBGM;
};