#pragma once

// ゲーム内で使用するサウンド ID
// BGM と SE をまとめて管理する
enum SoundID
{
    // ===== BGM =====
    BGM_TITLE,   // タイトル画面
    BGM_PLAY,    // プレイ中
    BGM_CLEAR,   // クリア画面

    // ===== SE =====
    SE_COUNT,        // カウントダウン
    SE_START,        // スタート表示
    SE_DECIDE,       // 決定
    SE_RING,         // リング取得
    SE_BACK_WORD,    // 文字後退
    SE_SKIP_WORD,    // 文字スキップ
    SE_ITEM_ACCEL,   // 加速アイテム取得
    SE_ITEM_SKIP,    // 促進アイテム取得
    SE_PORTAL,       // ポータル移動
    SE_ENEMY_SPAWN   // 敵出現
};