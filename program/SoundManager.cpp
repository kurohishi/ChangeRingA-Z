#include "SoundManager.h"

#include <map>

#include "DxLib.h"
#include "SoundID.h"

// 読み込んだサウンドハンドル一覧
std::map<int, int> SoundManager::sounds;

// 現在再生中の BGM の ID（何も再生していないときは -1）
int SoundManager::currentBGM = -1;

// サウンドデータを読み込む
void SoundManager::Load()
{
    // ===== BGM =====
    sounds[BGM_TITLE] = LoadSoundMem("sound/title_bgm.mp3");
    sounds[BGM_PLAY] = LoadSoundMem("sound/play_bgm.mp3");
    sounds[BGM_CLEAR] = LoadSoundMem("sound/clear_bgm.mp3");

    // ===== SE =====
    sounds[SE_COUNT] = LoadSoundMem("sound/count_se.mp3");
    sounds[SE_START] = LoadSoundMem("sound/start_se.mp3");
    sounds[SE_DECIDE] = LoadSoundMem("sound/decide_se.mp3");
    sounds[SE_RING] = LoadSoundMem("sound/ring_se.mp3");
    sounds[SE_BACK_WORD] = LoadSoundMem("sound/back_word_se.mp3");
    sounds[SE_SKIP_WORD] = LoadSoundMem("sound/skip_word_se.mp3");
    sounds[SE_ITEM_ACCEL] = LoadSoundMem("sound/item_accel_se.mp3");
    sounds[SE_ITEM_SKIP] = LoadSoundMem("sound/item_skip_se.mp3");
    sounds[SE_PORTAL] = LoadSoundMem("sound/warp_se.mp3");
    sounds[SE_ENEMY_SPAWN] = LoadSoundMem("sound/ring_se.mp3");
}

// サウンドデータを消去する
void SoundManager::Release()
{
    for (auto& sound_pair : sounds)
    {
        DeleteSoundMem(sound_pair.second);
    }

    sounds.clear();
    currentBGM = -1;
}

// BGM を再生する
void SoundManager::PlayBGM(int sound_id)
{
    // 同じ BGM がすでに再生中なら何もしない
    if (currentBGM == sound_id) return;

    StopBGM();

    int sound_handle = sounds[sound_id];
    ChangeVolumeSoundMem(100, sound_handle);

    PlaySoundMem(sound_handle, DX_PLAYTYPE_LOOP);
    currentBGM = sound_id;
}

// 再生中の BGMを停止する
void SoundManager::StopBGM()
{
    if (currentBGM >= 0){
        StopSoundMem(sounds[currentBGM]);
        currentBGM = -1;
    }
}

// SE を再生する
void SoundManager::PlaySE(int sound_id)
{
    int sound_handle = sounds[sound_id];
    ChangeVolumeSoundMem(100, sound_handle);
    PlaySoundMem(sound_handle, DX_PLAYTYPE_BACK);
}