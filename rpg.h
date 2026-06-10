#ifndef RPG_H
#define RPG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARACTERS 100 // 最大キャラクター数
#define MAX_RECORDS 100    // 最大レコード数
#define MAX_SPELLS 10      // 1キャラクターが使える最大魔法数
#define MAX_SPELL_TEXT 128 // CSVで管理する魔法リストの最大文字数

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
    char spells[MAX_SPELL_TEXT];
} Character;

typedef struct
{
    char name[20];
    int mp_cost;
    int power;
} Spell;

static const Spell SPELL_FIRE = {"ファイア", 5, 35};
static const Spell SPELL_ICE = {"アイス", 6, 40};
static const Spell SPELL_THUNDER = {"サンダー", 8, 50};
static const Spell SPELL_HOLY = {"ホーリー", 10, 65};
static const Spell SPELL_DARK = {"ダーク", 9, 60};

void print_status(Character *ch);

const Spell *find_spell_by_name(const char *name)
{
    if (strcmp(name, SPELL_FIRE.name) == 0)
        return &SPELL_FIRE;
    if (strcmp(name, SPELL_ICE.name) == 0)
        return &SPELL_ICE;
    if (strcmp(name, SPELL_THUNDER.name) == 0)
        return &SPELL_THUNDER;
    if (strcmp(name, SPELL_HOLY.name) == 0)
        return &SPELL_HOLY;
    if (strcmp(name, SPELL_DARK.name) == 0)
        return &SPELL_DARK;

    return NULL;
}

void trim_spell_name(char *text)
{
    char *start = text;
    while (*start == ' ' || *start == '\t')
        start++;

    if (start != text)
        memmove(text, start, strlen(start) + 1);

    int len = (int)strlen(text);
    while (len > 0 && (text[len - 1] == ' ' || text[len - 1] == '\t' || text[len - 1] == '\r'))
    {
        text[len - 1] = '\0';
        len--;
    }
}

int get_available_spells(const Character *ch, const Spell *spells[], int max_spells)
{
    int count = 0;
    char spell_text[MAX_SPELL_TEXT];

    if (ch->spells[0] == '\0')
        return 0;

    strncpy(spell_text, ch->spells, sizeof(spell_text) - 1);
    spell_text[sizeof(spell_text) - 1] = '\0';

    char *token = strtok(spell_text, "|");
    while (token != NULL && count < max_spells)
    {
        trim_spell_name(token);
        const Spell *spell = find_spell_by_name(token);
        if (spell != NULL)
            spells[count++] = spell;

        token = strtok(NULL, "|");
    }

    return count;
}

int get_castable_spells(const Character *ch, const Spell *spells[], int max_spells)
{
    const Spell *available[MAX_SPELLS];
    int available_count = get_available_spells(ch, available, MAX_SPELLS);
    int count = 0;

    for (int i = 0; i < available_count && count < max_spells; i++)
    {
        if (ch->mp >= available[i]->mp_cost)
            spells[count++] = available[i];
    }

    return count;
}

int choose_random_alive_index(Character *characters, int count)
{
    int alive_indexes[MAX_CHARACTERS];
    int alive_count = 0;

    for (int i = 0; i < count; i++)
    {
        if (characters[i].status == 0)
            alive_indexes[alive_count++] = i;
    }

    if (alive_count == 0)
        return -1;

    return alive_indexes[rand() % alive_count];
}

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

    char line[384];

    while (fgets(line, sizeof(line), fp) && count < max_records)
    {
        // 改行削除
        line[strcspn(line, "\n")] = '\0';

        // コメント行をスキップ
        if (line[0] == '#' || line[0] == '\0')
            continue;

        party[count].spells[0] = '\0';
        int items = sscanf(line, "%d,%d,%19[^,],%d,%d,%d,%d,%d,%d,%d,%f,%d,%f,%f,%f,%127[^\n]",
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
                           &party[count].drop_exp,
                           party[count].spells);

        if (items >= 15)
        {
            trim_spell_name(party[count].spells);
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

void cast_spell(Character *caster, Character *target, const Spell *spell)
{
    if (caster->mp < spell->mp_cost)
    {
        printf("MPが足りません\n");
        printf("--------------------------------\n");
        return;
    }

    printf("  %sは%sを唱えた！\n", caster->name, spell->name);
    caster->mp -= spell->mp_cost;

    int damage = spell->power + caster->level * 2 - target->defense;
    if (damage < 0)
        damage = 0;

    target->hp -= damage;
    if (target->hp < 0)
        target->hp = 0;

    printf("  %sに %d ダメージ！\n", target->name, damage);
    printf("  %sの残りHP: %d\n", target->name, target->hp);
    printf("  %sの残りMP: %d\n", caster->name, caster->mp);

    if (target->hp == 0)
    {
        printf("  ★ %sは倒れた！\n", target->name);
        target->status = 1;
    }

    printf("--------------------------------\n");
}

void command(Character *my_character, Character *target_character, int myparty_count, int eneParty_count, int cmd)
{
    (void)myparty_count;

    int target_index;
    switch (cmd)
    {
    case 0:
        // 情報確認コマンド
        print_status(my_character);
        printf("生存している対象:\n");
        for (int i = 0; i < eneParty_count; i++)
        {
            if (target_character[i].status == 0)
            {
                printf("  %d: %s (HP: %d)\n", i, target_character[i].name, target_character[i].hp);
            }
        }
        printf("--------------------------------\n");
        int choose_cmd;
        printf("  コマンドを選択してください:\n");
        printf("  0: 情報確認 | 1: 攻撃 | 2: 魔法 | 3: 防御 | 4: 回避\n");
        printf("  選択: ");
        scanf("%d", &choose_cmd);
        command(my_character, target_character, myparty_count, eneParty_count, choose_cmd);
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
                return;
            }
            attack(my_character, &target_character[target_index]);
        }
        else
        {
            target_index = choose_random_alive_index(target_character, eneParty_count);
            if (target_index < 0)
                return;

            attack(my_character, &target_character[target_index]);
        }
        break;
    case 2:
        // 魔法コマンド
        if (my_character->type == 0)
        {
            const Spell *spells[MAX_SPELLS];
            int spell_count = get_available_spells(my_character, spells, MAX_SPELLS);
            int spell_index;

            if (spell_count == 0)
            {
                printf("%sは魔法を覚えていません\n", my_character->name);
                printf("--------------------------------\n");
                break;
            }

            printf("使用する魔法を選択してください:\n");
            for (int i = 0; i < spell_count; i++)
            {
                printf("%d: %s (消費MP: %d)\n", i, spells[i]->name, spells[i]->mp_cost);
            }
            printf("選択: ");
            scanf("%d", &spell_index);
            if (spell_index < 0 || spell_index >= spell_count)
            {
                printf("無効な選択です\n");
                command(my_character, target_character, myparty_count, eneParty_count, cmd);
                return;
            }

            printf("魔法対象を選択してください:\n");
            for (int i = 0; i < eneParty_count; i++)
            {
                if (target_character[i].status == 0)
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
                return;
            }

            cast_spell(my_character, &target_character[target_index], spells[spell_index]);
        }
        else
        {
            const Spell *spells[MAX_SPELLS];
            int spell_count = get_castable_spells(my_character, spells, MAX_SPELLS);

            if (spell_count == 0)
            {
                target_index = choose_random_alive_index(target_character, eneParty_count);
                if (target_index < 0)
                    return;

                attack(my_character, &target_character[target_index]);
                break;
            }

            target_index = choose_random_alive_index(target_character, eneParty_count);
            if (target_index < 0)
                return;

            const Spell *spell = spells[rand() % spell_count];
            cast_spell(my_character, &target_character[target_index], spell);
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
    printf("  魔法: %s\n", ch->spells[0] == '\0' ? "なし" : ch->spells);
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
