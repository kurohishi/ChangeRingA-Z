#include <Windows.h>
#include "DxLib.h"
#include "Game.h"
#include "Constants.h"
#include "SoundManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // ウィンドウモードで起動する
    ChangeWindowMode(TRUE);

    // 画面サイズを設定する
    SetGraphMode(GameConst::kScreenWidth, GameConst::kScreenHeight, 32);

    // DxLib を初期化する
    if (DxLib_Init() == -1)
    {
        return -1;
    }

    // 裏画面に描画する
    SetDrawScreen(DX_SCREEN_BACK);

    // サウンドを読み込む
    SoundManager::Load();

    // ゲーム本体を生成する
    Game game;

    // メインループ
    while (ProcessMessage() == 0)
    {
        // 画面をクリアする
        ClearDrawScreen();

        // ゲームを更新する
        game.Update();

        // ゲームを描画する
        game.Draw();

        // 裏画面を表に反映する
        ScreenFlip();
    }

    // サウンドを解放する
    SoundManager::Release();

    // DxLib を終了する
    DxLib_End();

    return 0;
}