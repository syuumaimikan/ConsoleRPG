#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include "rpg.h"

int main(void)
{
    // プレイヤーパーティーとエネミーの配列を用意する
    Character party[MAX_CHARACTERS];
    Character enemy[MAX_CHARACTERS];

    int num_party = 0; // 読み込んだプレイヤー数
    int num_enemy = 0; // 読み込んだエネミー数
    int turn = 1;      // 現在のターン数

    // 乱数の初期化
    srand((unsigned int)time(NULL));

    // ===== CSVからキャラクターを読み込む =====
    num_party = load_characters("characters.csv", party, MAX_RECORDS);
    num_enemy = load_characters("enemies.csv", enemy, MAX_RECORDS);

    if (num_party < 0)
    {
        fprintf(stderr, "キャラクターの読み込みに失敗しました\n");
        return EXIT_FAILURE;
    }
    if (num_enemy < 0)
    {
        fprintf(stderr, "エネミーの読み込みに失敗しました\n");
        return EXIT_FAILURE;
    }

    printf("読み込んだキャラクター数: %d\n", num_party);
    printf("読み込んだエネミー数: %d\n", num_enemy);

    // ===== 読み込んだ内容を確認表示する =====
    for (int i = 0; i < num_party; i++)
    {
        printf("キャラクター名: %s, レベル: %d, HP: %d, MP: %d, "
               "攻撃: %d, 防御: %d, 回避率: %.2f, 素早さ: %d, "
               "クリティカル率: %.2f, クリティカル倍率: %.1f, 魔法: %s\n",
               party[i].name,
               party[i].level,
               party[i].hp,
               party[i].mp,
               party[i].attack,
               party[i].defense,
               party[i].dodge_rate,
               party[i].speed,
               party[i].critical_rate,
               party[i].critical_damage,
               party[i].spells[0] == '\0' ? "なし" : party[i].spells);
    }

    for (int i = 0; i < num_enemy; i++)
    {
        printf("エネミー名: %s, レベル: %d, HP: %d, MP: %d, "
               "攻撃: %d, 防御: %d, 回避率: %.2f, 素早さ: %d, "
               "クリティカル率: %.2f, クリティカル倍率: %.1f, 魔法: %s\n",
               enemy[i].name,
               enemy[i].level,
               enemy[i].hp,
               enemy[i].mp,
               enemy[i].attack,
               enemy[i].defense,
               enemy[i].dodge_rate,
               enemy[i].speed,
               enemy[i].critical_rate,
               enemy[i].critical_damage,
               enemy[i].spells[0] == '\0' ? "なし" : enemy[i].spells);
    }

    // ===== 行動順リスト作成 =====
    // 全員（プレイヤー＋エネミー）を1つのリストにまとめて素早さ順に並べることで行動順を決める
    int total = num_party + num_enemy;
    TurnOrder order[2 * MAX_CHARACTERS];

    // プレイヤーをリストに追加する
    for (int i = 0; i < num_party; i++)
    {
        order[i].index = i;              // party[] の番号
        order[i].type = TYPE_PLAYER;     // プレイヤーであることを示す
        order[i].speed = party[i].speed; // 素早さを保存
    }

    // エネミーをリストに追加する（プレイヤーの続きから格納）
    for (int i = 0; i < num_enemy; i++)
    {
        order[num_party + i].index = i;
        order[num_party + i].type = TYPE_ENEMY;
        order[num_party + i].speed = enemy[i].speed;
    }

    // ===== メインゲームループ =====
    int game_over = 0;
    printf("ゲーム開始！\n");

    while (!game_over)
    {
        // 素早さ順に並べ替えて行動順を決める
        for (int i = 0; i < num_party; i++)
        {
            order[i].speed = party[i].speed + party[i].speed_buff;
        }

        for (int i = 0; i < num_enemy; i++)
        {
            order[num_party + i].speed = enemy[i].speed + enemy[i].speed_buff;
        }

        sort_order_by_speed(order, total);

        print_turn_start(turn);

        // 行動順にキャラクターを動かす
        for (int i = 0; i < total; i++)
        {
            int index = order[i].index;
            int type = order[i].type;

            // 戦闘不能のキャラクターはスキップする
            if (type == TYPE_PLAYER && party[index].status == STATUS_DEAD)
                continue;
            if (type == TYPE_ENEMY && enemy[index].status == STATUS_DEAD)
                continue;

            // 行動するキャラクターと対象・味方を決める
            Character *actor = (type == TYPE_PLAYER) ? &party[index] : &enemy[index];
            Character *targets = (type == TYPE_PLAYER) ? enemy : party;
            Character *allies = (type == TYPE_PLAYER) ? party : enemy;
            int target_count = (type == TYPE_PLAYER) ? num_enemy : num_party;
            int ally_count = (type == TYPE_PLAYER) ? num_party : num_enemy;

            print_actor_turn(actor);

            if (type == TYPE_PLAYER)
            {
                // プレイヤーはコマンドを入力して行動する
                int cmd;
                printf("  コマンドを選択してください:\n");
                printf("  0: 情報確認 | 1: 攻撃 | 2: 魔法 | 3: 防御 | 4: 回避\n");
                printf("  選択: ");
                scanf("%d", &cmd);
                command(actor, targets, target_count, allies, ally_count, cmd);
                update_buffs(actor);
            }
            else
            {
                // エネミーはAIが自動でコマンドを選ぶ
                // MPが足りる魔法があれば50%の確率で魔法を使い、それ以外は攻撃する
                const Spell *castable_spells[MAX_SPELLS];
                int castable_count = get_castable_spells(actor, castable_spells, MAX_SPELLS);
                int cmd = (castable_count > 0 && rand() % 2 == 0) ? 2 : 1;
                command(actor, targets, target_count, allies, ally_count, cmd);
                update_buffs(actor);
            }

            // ===== ゲーム終了条件を確認する =====

            // プレイヤーが全員倒れたか数える
            int dead_players = 0;
            for (int j = 0; j < num_party; j++)
            {
                if (party[j].status == STATUS_DEAD)
                    dead_players++;
            }

            // エネミーが全員倒れたか数える
            int dead_enemies = 0;
            for (int j = 0; j < num_enemy; j++)
            {
                if (enemy[j].status == STATUS_DEAD)
                    dead_enemies++;
            }

            // 全員倒れていたらゲーム終了
            if (dead_players == num_party)
            {
                printf("全てのプレイヤーが倒れました！\n");
                game_over = 1;
                break;
            }
            if (dead_enemies == num_enemy)
            {
                printf("全てのエネミーが倒れました！\n");
                game_over = 1;
                break;
            }

            Sleep(1000); // 1秒待つ（読みやすくするため）
        }

        turn++;
        Sleep(1000);
    }

    return 0;
}