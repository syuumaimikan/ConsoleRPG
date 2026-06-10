#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include "rpg.h"

int main(void)
{
    Character party[MAX_CHARACTERS];
    Character enemy[MAX_CHARACTERS];

    int num_characters = 0;
    int num_enemys = 0;

    int turn = 1;

    int order[2 * MAX_CHARACTERS][3]; // 0:インデックス, 1:素早さ,2:タイプ(0:プレイヤー, 1:エネミー)

    num_characters = load_characters("characters.csv", party, MAX_RECORDS);
    num_enemys = load_characters("enemies.csv", enemy, MAX_RECORDS);

    if (num_characters < 0)
    {
        fprintf(stderr, "キャラクターの読み込みに失敗しました\n");
        return EXIT_FAILURE;
    }
    if (num_enemys < 0)
    {
        fprintf(stderr, "エネミーの読み込みに失敗しました\n");
        return EXIT_FAILURE;
    }
    printf("読み込んだキャラクター数: %d\n", num_characters);
    printf("読み込んだエネミー数: %d\n", num_enemys);

    for (int i = 0; i < num_characters; i++)
    {
        printf("キャラクター名: %s, レベル: %d, HP: %d, MP: %d, 攻撃: %d, 防御: %d, 回避率: %.2f, 素早さ: %d, クリティカル率: %.2f, クリティカルダメージ: %.1f\n",
               party[i].name,
               party[i].level,
               party[i].hp,
               party[i].mp,
               party[i].attack,
               party[i].defense,
               party[i].evaluation,
               party[i].speed,
               party[i].cc,
               party[i].cd);
    }

    for (int i = 0; i < num_enemys; i++)
    {
        printf("エネミー名: %s, レベル: %d, HP: %d, MP: %d, 攻撃: %d, 防御: %d, 回避率: %.2f, 素早さ: %d, クリティカル率: %.2f, クリティカルダメージ: %.1f\n",
               enemy[i].name,
               enemy[i].level,
               enemy[i].hp,
               enemy[i].mp,
               enemy[i].attack,
               enemy[i].defense,
               enemy[i].evaluation,
               enemy[i].speed,
               enemy[i].cc,
               enemy[i].cd);
    }

    for (int i = 0; i < num_characters; i++)
    {
        order[i][0] = i;              // インデックス
        order[i][1] = party[i].speed; // 素早さ
        order[i][2] = 0;              // タイプ: プレイヤー
    }

    for (int i = 0; i < num_enemys; i++)
    {
        order[num_characters + i][0] = i;              // インデックス
        order[num_characters + i][1] = enemy[i].speed; // 素早さ
        order[num_characters + i][2] = 1;              // タイプ: エネミー
    }

    int game_over = 0;

    printf("ゲーム開始！\n");

    while (!game_over)
    {
        sort_order(order, num_characters + num_enemys);

        print_turn_start(turn);

        for (int i = 0; i < num_characters + num_enemys; i++)
        {
            int index = order[i][0];
            int type = order[i][2];
            int choose_cmd;

            // 死亡キャラはスキップ
            if (type == 0 && party[index].status == 1)
                continue;
            if (type == 1 && enemy[index].status == 1)
                continue;

            Character *actor = (type == 0) ? &party[index] : &enemy[index];
            Character *targets = (type == 0) ? enemy : party;
            int target_count = (type == 0) ? num_enemys : num_characters;

            print_actor_turn(actor);

            if (type == 0)
            {
                printf("  コマンドを選択してください:\n");
                printf("  0: 情報確認 | 1: 攻撃 | 2: 魔法 | 3: 防御 | 4: 回避\n");
                printf("  選択: ");
                scanf("%d", &choose_cmd);
                command(&party[index], enemy, num_characters, num_enemys, choose_cmd);
            }
            else
            {
                command(&enemy[index], party, num_enemys, num_characters, 1);
            }

            // ゲーム終了条件のチェック
            int all_players_dead = 0, all_enemys_dead = 0;
            for (int j = 0; j < num_characters; j++)
            {
                if (party[j].status == 1)
                    all_players_dead++;
            }
            for (int j = 0; j < num_enemys; j++)
            {
                if (enemy[j].status == 1)
                    all_enemys_dead++;
            }

            if (all_players_dead == num_characters)
            {
                printf("全てのプレイヤーが倒れました！\n");
                game_over = 1;
                break; // for を抜ける
            }
            if (all_enemys_dead == num_enemys)
            {
                printf("全てのエネミーが倒れました！\n");
                game_over = 1;
                break; // for を抜ける
            }

            Sleep(1000);
        }

        turn++;
        Sleep(1000);
    }

    return 0;
}