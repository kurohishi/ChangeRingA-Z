#pragma once

#include <map>

// サウンドの読み込み・再生・解放を管理するクラス
class SoundManager
{
public:
    // ===== 初期化・解放 =====

    // サウンドデータを読み込む
    static void Load();

    // サウンドデータを解放する
    static void Release();

    // ===== BGM 再生 =====

    // BGM を再生する
    static void PlayBGM(int sound_id);

    // 再生中の BGM を停止する
    static void StopBGM();

    // ===== SE 再生 =====

    // 効果音を再生する
    static void PlaySE(int sound_id);

private:
    // 読み込んだサウンドハンドル
    static std::map<int, int> sounds;

    // 現在再生中の BGM ID
    static int currentBGM;
};