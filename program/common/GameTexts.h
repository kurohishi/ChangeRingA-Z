#pragma once

namespace GameText
{
    // ========================================
    // 共通
    // ========================================
    namespace Common
    {
        inline constexpr const char* kStart = "Start!";
    }

    // ========================================
    // 共通UI
    // ========================================
    namespace UI
    {
        inline constexpr const char* kSkip = "[R]でスキップ";
        inline constexpr const char* kPressEnter = "Press Enter!";
        inline constexpr const char* kYes = "Yes";
        inline constexpr const char* kNo = "No";
    }

    // ========================================
    // タイトル
    // ========================================
    namespace Title
    {
        inline constexpr const char* kLogoMain = "ChangeRing O";
        inline constexpr const char* kLogoSub = "from A to Z";
        inline constexpr const char* kExitMessage = "ゲームを終了しますか？";
    }

    // ========================================
    // ガイド
    // ========================================
    namespace Guide
    {
        inline constexpr const char* kTitle = "遊び方";
        inline constexpr const char* kTitleDecorated = "- GAME GUIDE -";
        inline constexpr const char* kStart = "Press Enter to Start";

        inline constexpr const char* kLine1 =
            "① プレイヤー移動：W / A / S / D";

        inline constexpr const char* kLine2 =
            "② リング 〇に入ると、アルファベット『A』が進む（A→B→…）";

        inline constexpr const char* kLine3 =
            "③ 敵△と重なると、アルファベットが１つ戻る（C→B→A）";

        inline constexpr const char* kLine4 =
            "④ アルファベット『Z』で〇に入るとゲームクリア！";

        inline constexpr const char* kLine5 =
            "⑤ 〈他の世界〉と重なる位置に〇があるとき、世界が変化する";
    }

    // ========================================
    // リジューム
    // ========================================
    namespace Resume
    {
        inline constexpr const char* kTitle = "- ACCESS POINT -";
        inline constexpr const char* kContinue = "Continue";
        inline constexpr const char* kNewGame = "New Game";
    }

    // ========================================
    // クリア
    // ========================================
    namespace Clear
    {
        inline constexpr const char* kTitle = "GAME CLEAR!";
        inline constexpr const char* kHint = "Enter : Retry   B : Title";
        inline constexpr const char* kTimeFormat = "CLEAR TIME : %.2f sec";
    }

    // ========================================
    // チュートリアル
    // ========================================
    namespace Tutorial
    {
        inline constexpr const char* kTitle = "チュートリアル";
        inline constexpr const char* kSkipConfirm =
            "チュートリアルをスキップしますか？";

        // ===== 目覚め =====
        namespace WakeUp
        {
            inline constexpr const char* kTalks[] = {
                "………",
                "うっ……ここは？",
                "俺は、確か……",
                "…っ！？",
                "かっ、身体が…",
                "アルファベットの『A』になってるっ！？",
                "………",
                "もしかして、俺は夢でも見ているのか？",
                "………",
                "はぁ…悩んでいても、しょうがないか…",
                "なぜだか、動くことはできそうだし",
                "とりあえず、先へ進んでよう！"
            };
        }

        // ===== リング発見 =====
        namespace NoticeRing
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "んっ……！？",
                "あれは何だ？",
                "光の…リング？"
            };
        }

        // ===== 最初のリングを踏む前 =====
        namespace ConsiderFirstRing
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "なにか不思議な力を感じるぞ…",
                "ここを通らないと、先には進めそうにないし",
                "この『光のリング』を踏んでみるか…"
            };
        }

        // ===== 最初のリングを踏んだ後 =====
        namespace ReactToFirstRing
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "身体が『A』から『B』に変わった！？",
                "ってことは…",
                "『Z』で入ったら、どうなるんだ？",
                "あのリングは、向こう側へ移動したみたいだけど…",
                "………",
                "もしかして、あの「青い四角」に入ったら",
                "向こう側へ行けたりして…"
            };
        }

        // ===== ポータル =====
        namespace ReactToPortal
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "あっ、本当に移動できた！",
                "この「青い四角」はワープポータルだったのか…",
                "他にできることもないし",
                "ひとまず『Z』を目指して進んでみるか"
            };
        }

        // ===== 世界変化 =====
        namespace ReactToWorldShift
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "はっ……！？",
                "どうなっているんだ…？",
                "今度は「身体」だけじゃなくて",
                "「世界」も変わった！？",
                "本当にどうなっているんだよ…全く",
                "…………",
                "ん………あれは？"
            };
        }

        // ===== 敵警戒 =====
        namespace WarnAboutEnemy
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "見るからに怪しい雰囲気を感じるな…",
                "コレは通って大丈夫なのか…？",
                "なんだか嫌な予感がするぞ"
            };
        }

        // ===== 敵に当たった直後 =====
        namespace ReactToEnemyHit
        {
            namespace StayA
            {
                inline constexpr const char* kTalks[] = {
                    nullptr,
                    "戻っちまった…",
                    "しかも、何も考えずに何度も踏んだから",
                    "最初からやり直しか…",
                    "ただ『A』で踏んでも何も起きなかったし",
                    "それが分かっただけでも良しとしよう"
                };
            }

            namespace BtoA
            {
                inline constexpr const char* kTalks[] = {
                    nullptr,
                    "くそ………やっぱり、こうなったか",
                    "しかも２回踏んだから『A』に戻ってしまった",
                    "もし、もう１度踏んでしまったら…",
                    "………",
                    "ちょっと怖いし、これからは気を付けないと…"
                };
            }

            namespace CtoB
            {
                inline constexpr const char* kTalks[] = {
                    nullptr,
                    "………やっぱり",
                    "この赤いマークを踏むと",
                    "アルファベットが戻ってしまうのか…",
                    "しかも、あそこのマークは動くみたいだし",
                    "これからは気を付けて進まないとな…"
                };
            }
        }

        // ===== 敵地帯を抜けた後の振り返り =====
        namespace ReflectOnEnemyTrap
        {
            namespace StayA
            {
                inline constexpr const char* kTalks[] = {
                    nullptr,
                    "結局、何度も踏んでしまった…",
                    "………まあ、いっか……",
                    "深く考えてもしょうがないし",
                    "『Z』になるまで長くなりそうだしな",
                    "とりあえず、進んどけば何とかなるでしょ！"
                };
            }

            namespace BtoA
            {
                inline constexpr const char* kTalks[] = {
                    nullptr,
                    "結局『A』に戻ってしまった…か",
                    "…まぁ、深く考えてもしょうがないし",
                    "ここから仕切り直すか…",
                    "ただこれ以上何かあっても嫌だし",
                    "油断せずに進まないと…"
                };
            }

            namespace CtoB
            {
                inline constexpr const char* kTalks[] = {
                    nullptr,
                    "ふぅ……",
                    "なんとか踏まずにここまで来れた！",
                    "とはいえ『Z』までは先が長そうだし",
                    "まだまだ何が起こるか分からない",
                    "これからも油断せずに進まないと…"
                };
            }
        }

        // ===== 追跡敵登場 =====
        namespace NoticeRingChaser
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "ん…？",
                "今度は、一体どんなヤツだ？"
            };
        }

        namespace UnderstandRingChaser
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "あいつ、まさか…",
                "リングを追いかけて移動しているのか？"
            };
        }

        namespace ConfirmRingChaserBehavior
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "やっぱり、そうだったのか…"
            };
        }

        // ===== 追い詰められ =====
        namespace FeelCornered
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "くそ、このままじゃ…",
                "いつまで経っても先に進めない…",
                "一体…どうすれば…",
                "ん…？　あれは、何だ？"
            };
        }

        // ===== アイテム =====
        namespace ReactToMysteriousItem
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "触れても、特に何も起きなかったけど…"
            };
        }

        namespace UnderstandSpeedBoost
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "なるほど…",
                "アイツよりも少しだけ速く動けるようになったみたいだ",
                "これなら何とかなるかもしれないぞ"
            };
        }

        // ===== 崩壊イベント =====
        namespace CollapseWarning
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "よし、この調子でどんどん進んでいこう！",
                "（グラッ…）",
                "うっ…急に意識が…",
                "くそっ、これからってときに…",
                "時間をかけすぎちまったって…ことなの…かっ…",
                "……………"
            };
        }

        // ===== リセット後 =====
        namespace WakeAfterReset
        {
            inline constexpr const char* kTalks[] = {
                nullptr,
                "……………",
                "うっ……ここは…",
                "……………",
                "やっぱり…『A』に戻ってしまったか",
                "………まあ、仕方ねぇな",
                "気を取り直して、また『Z』を目指そう！"
            };
        }
    }
}