#ifndef RPG_H
#define RPG_H

#define MAX_CHARACTERS 100 // 最大キャラクター数
#define MAX_RECORDS 100    // 最大レコード数

typedef struct
{
    int status; // 0:生存, 1:死亡
    int type;   // 0:プレイヤー, 1:エネミー
    char name[20];
    int level;
    int hp;
    int max_hp;
    int mp;
    int max_mp;
    int attack;
    int defense;
    float evaluation;
    int speed;
    float cc;
    float cd;
    float drop_exp;
} Character;

// パーティーCSV読み込み関数
int load_characters(const char *filename, Character *party, int max_records)
{
    int count = 0;
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("ファイルを開けませんでした");
        return -1;
    }

    char line[256];

    while (fgets(line, sizeof(line), fp) && count < max_records)
    {
        // 改行削除
        line[strcspn(line, "\n")] = '\0';

        // コメント行をスキップ
        if (line[0] == '#' || line[0] == '\0')
            continue;

        if (sscanf(line, "%d,%d,%19[^,],%d,%d,%d,%d,%d,%d,%d,%f,%d,%f,%f,%f",
                   &party[count].status,
                   &party[count].type,
                   party[count].name,
                   &party[count].level,
                   &party[count].hp,
                   &party[count].max_hp,
                   &party[count].mp,
                   &party[count].max_mp,
                   &party[count].attack,
                   &party[count].defense,
                   &party[count].evaluation,
                   &party[count].speed,
                   &party[count].cc,
                   &party[count].cd,
                   &party[count].drop_exp) == 15)
        {
            count++;
        }
    }

    fclose(fp);
    return count;
}

void sort_order(int order[][3], int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (order[j][1] < order[j + 1][1]) // 素早さで降順ソート
            {
                int temp_index = order[j][0];
                int temp_speed = order[j][1];
                int temp_type = order[j][2];

                order[j][0] = order[j + 1][0];
                order[j][1] = order[j + 1][1];
                order[j][2] = order[j + 1][2];

                order[j + 1][0] = temp_index;
                order[j + 1][1] = temp_speed;
                order[j + 1][2] = temp_type;
            }
        }
    }
}

void attack(Character *attacker, Character *defender)
{
    printf("  %sの攻撃！\n", attacker->name);

    int damage = attacker->attack - defender->defense;
    if (damage < 0)
        damage = 0;

    defender->hp -= damage;
    if (defender->hp < 0)
        defender->hp = 0;

    printf("  %sに %d ダメージ！\n", defender->name, damage);
    printf("  %sの残りHP: %d\n", defender->name, defender->hp);

    if (defender->hp == 0)
    {
        printf("  ★ %sは倒れた！\n", defender->name);
        defender->status = 1;
    }

    printf("--------------------------------\n");
}

void command(Character *my_character, Character *target_character, int myparty_count, int eneParty_count, int cmd)
{
    int target_index;
    switch (cmd)
    {
    case 0:
        // 情報確認コマンド
        print_status(my_character);
        printf("生存している仲間:\n");
        for (int i = 0; i < myparty_count; i++)
        {
            if (target_character[i].status == 0) // 生存している仲間を表示
            {
                printf("  %d: %s (HP: %d)\n", i, target_character[i].name, target_character[i].hp);
            }
        }
        printf("--------------------------------\n");
        printf("生存しているエネミー:\n");
        for (int i = 0; i < eneParty_count; i++)
        {
            if (target_character[i].status == 0) // 生存しているエネミーを表示
            {
                printf("  %d: %s (HP: %d)\n", i, target_character[i].name, target_character[i].hp);
            }
        }
        break;
    case 1:
        // 攻撃コマンド
        if (my_character->type == 0)
        {
            // プレイヤーの攻撃処理
            printf("攻撃対象を選択してください:\n");
            for (int i = 0; i < eneParty_count; i++)
            {
                if (target_character[i].status == 0) // 生存しているエネミーを表示
                {
                    printf("%d: %s (HP: %d)\n", i, target_character[i].name, target_character[i].hp);
                }
            }
            printf("選択: ");
            scanf("%d", &target_index);
            if (target_index < 0 || target_index >= eneParty_count || target_character[target_index].status != 0)
            {
                printf("無効な選択です\n");
                command(my_character, target_character, myparty_count, eneParty_count, cmd);
            }
            attack(my_character, &target_character[target_index]);
        }
        else
        {
            // エネミーの攻撃処理　生存しているプレイヤーからランダムで一体を攻撃
            int alive_players[MAX_CHARACTERS];
            int alive_count = 0;
            for (int i = 0; i < myparty_count; i++)
            {
                if (target_character[i].status == 0) // 生存しているプレイヤーをリストに追加
                {
                    alive_players[alive_count++] = i;
                }
            }
            if (alive_count == 0)
                return; // もう攻撃対象がいない
            int random_index = rand() % alive_count;
            attack(my_character, &target_character[alive_players[random_index]]);
        }
        break;
    case 2:
        // 魔法コマンド（仮実装）
        if (my_character->type == 0)
        {
            printf("魔法はまだ実装されていません\n");
        }
        else
        {
            // エネミーの魔法攻撃処理　生存しているプレイヤーからランダムで一体を攻撃
            int alive_players[MAX_CHARACTERS];
            int alive_count = 0;
            for (int i = 0; i < myparty_count; i++)
            {
                if (target_character[i].status == 0) // 生存しているプレイヤーをリストに追加
                {
                    alive_players[alive_count++] = i;
                }
            }
            if (alive_count == 0)
                return; // もう攻撃対象がいない
            int random_index = rand() % alive_count;
            printf("エネミーは魔法攻撃！\n");
            attack(my_character, &target_character[alive_players[random_index]]);
        }
        break;
    case 3:
        // 防御コマンド（仮実装）
        printf("防御はまだ実装されていません\n");
        break;
    case 4:
        // 回避コマンド（仮実装）
        printf("回避はまだ実装されていません\n");
        break;
    default:
        printf("無効なコマンドです\n");
        command(my_character, target_character, myparty_count, eneParty_count, cmd);
        break;
    }
}

void print_status(Character *ch)
{
    printf("【%s】のステータス:\n", ch->name);
    printf("  HP: %d/%d\n", ch->hp, ch->max_hp);
    printf("  MP: %d/%d\n", ch->mp, ch->max_mp);
    printf("  攻撃: %d\n", ch->attack);
    printf("  防御: %d\n", ch->defense);
    printf("  素早さ: %d\n", ch->speed);
    printf("  クリティカル率: %.2f\n", ch->cc);
    printf("  クリティカルダメージ: %.1f\n", ch->cd);
    printf("--------------------------------\n");
}

void print_turn_start(int turn)
{
    printf("\n==============================\n");
    printf("◆ %dターン目 開始\n", turn);
    printf("==============================\n\n");
}

void print_actor_turn(Character *ch)
{
    printf(">【%s】のターン（HP: %d）\n", ch->name, ch->hp);
}

#endif /* RPG_H */