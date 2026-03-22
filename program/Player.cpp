#include "Player.h"

#include "DxLib.h"

Player::Player(const Map& map) {

    // 初期位置（仮配置）
    x_ = 335;
    y_ = 240;

    // 初期位置が通行不可なら、最初に見つかった通行可能マスへ補正する
    if (!map.IsWalkable(x_, y_)) {
        map.GetFirstWalkablePos(x_, y_);
    }

    // リング関連の初期座標は現在位置に合わせておく
    ring_x_ = x_;
    ring_y_ = y_;

    // プレイヤーの初期状態
    character_ = 'A';
    speed_ = 1000;
    font_size_ = 24;
}

// タイル左上基準で位置を設定
void Player::SetTilePos(int tile_x, int tile_y) {

    x_ = tile_x * Map::TILE;
    y_ = tile_y * Map::TILE;
}

// タイル中心基準で位置を設定
void Player::SetTileCenterPos(int tile_x, int tile_y) {

    x_ = tile_x * Map::TILE + Map::TILE / 2;
    y_ = tile_y * Map::TILE + Map::TILE / 2;
}

//プレイヤー配置の更新
bool Player::Update(const Map& map) {

    const int previous_center_x = GetTileCenterX();
    const int previous_center_y = GetTileCenterY();

    int delta_x = 0;
    int delta_y = 0;

    if (CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_A)) {
        delta_x = -Map::TILE;
    }
    else if (CheckHitKey(KEY_INPUT_RIGHT) || CheckHitKey(KEY_INPUT_D)) {
        delta_x = Map::TILE;
    }
    else if (CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W)) {
        delta_y = -Map::TILE;
    }
    else if (CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S)) {
        delta_y = Map::TILE;
    }

    // 入力がなければ何もしない
    if (delta_x == 0 && delta_y == 0) {
        return false;
    }

    // 次に移動したいタイル中心座標を計算する
    const int next_center_x = GetTileCenterX() + delta_x;
    const int next_center_y = GetTileCenterY() + delta_y;

    // 当たり判定用に、移動後の矩形左上座標を求める
    const int next_left = next_center_x - width_ / 2;
    const int next_top = next_center_y - height_ / 2;

	// 移動先が通行可能かを判定する
    if (!map.IsWalkableRect(next_left, next_top, width_, height_)) {
        ring_x_ = 0;
        ring_y_ = 0;
        return false;
    }

    // 実際にプレイヤー座標を更新する
    x_ += delta_x / Map::TILE;
    y_ += delta_y / Map::TILE;

    // リング用には移動先の中心座標を保持しておく
    ring_x_ = next_center_x;
    ring_y_ = next_center_y;

    // 中心座標が変わったかどうかを返す
    return next_center_x != previous_center_x ||
        next_center_y != previous_center_y;
}

// プレイヤーを文字として描画する
void Player::Draw() {

    draw_center_x_ = GetTileCenterX();
    draw_center_y_ = GetTileCenterY();

    // プレイヤーは現在の文字で描画する
    char text[2] = { character_, '\0' };
    DrawString(draw_center_x_ - font_size_ / 10,
        draw_center_y_ - font_size_ / 4,
        text,
        GetColor(255, 255, 255));
}

// 描画位置が前回から変わったかを判定する
bool Player::HasDrawPositionChanged() const {

    return draw_center_x_ != prev_draw_center_x_ ||
        draw_center_y_ != prev_draw_center_y_;
}

// 現在の描画位置を「前回の描画位置」として保存する
void Player::CommitDrawPosition() {

    prev_draw_center_x_ = draw_center_x_;
    prev_draw_center_y_ = draw_center_y_;
}

// プレイヤーのレベルを上げる（アルファベットを１つ進む）
void Player::LevelUp() {

    if (character_ < 'Z') {
        ++character_;
    }
}

// プレイヤーのレベルを下げる（アルファベットを１つ戻す）
void Player::LevelDown() {

    if (character_ > 'A') {
        --character_;
    }
}

// プレイヤーのレベルが最大か
bool Player::IsZ() const {
    return character_ == 'Z';
}