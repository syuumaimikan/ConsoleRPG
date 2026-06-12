#ifndef RPG_H
#define RPG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==============================
// 定数の定義
// ==============================

#define MAX_CHARACTERS 100 // 最大キャラクター数（プレイヤー・エネミー両方）
#define MAX_RECORDS 100    // CSVから読み込める最大行数
#define MAX_SPELLS 10      // 1キャラクターが覚えられる最大魔法数
#define MAX_SPELL_TEXT 128 // 魔法リスト文字列の最大文字数（CSV1列分）

// 魔法の効果タイプ
#define SPELL_DAMAGE 0       // 単体ダメージ
#define SPELL_HEAL 1         // 単体回復
#define SPELL_DEATH 2        // 即死（確率）
#define SPELL_ALL_DAMAGE 3   // 全体ダメージ
#define SPELL_ATTACK_UP 4    // 攻撃力アップ
#define SPELL_DEFENSE_UP 5   // 防御力アップ
#define SPELL_SPEED_UP 6     // 素早さアップ
#define SPELL_DRAIN 7        // HP吸収
#define SPELL_ATTACK_DOWN 8  // 攻撃力ダウン
#define SPELL_DEFENSE_DOWN 9 // 防御力ダウン
#define SPELL_REGENE 10      // 継続回復（リジェネ）
#define SPELL_REVIVE 11      // 蘇生
#define SPELL_MP_DRAIN 12    // MP吸収
#define SPELL_MP_HEAL 13     // MP回復

// キャラクタータイプ
#define TYPE_PLAYER 0 // プレイヤー
#define TYPE_ENEMY 1  // エネミー

// ステータス
#define STATUS_ALIVE 0 // 生存中
#define STATUS_DEAD 1  // 戦闘不能

// ==============================
// 構造体の定義
// ==============================

// キャラクター1人分のデータ
typedef struct
{
    int status;                  // 生死（STATUS_ALIVE / STATUS_DEAD）
    int type;                    // 種別（TYPE_PLAYER / TYPE_ENEMY）
    char name[20];               // 名前
    int level;                   // レベル
    int hp;                      // 現在HP
    int max_hp;                  // 最大HP
    int mp;                      // 現在MP
    int max_mp;                  // 最大MP
    int attack;                  // 攻撃力
    int defense;                 // 防御力
    float dodge_rate;            // 回避率（0.0?1.0）
    int speed;                   // 素早さ（行動順に影響）
    float critical_rate;         // クリティカル率（0.0?1.0）
    float critical_damage;       // クリティカルダメージ倍率
    float drop_exp;              // 倒したときに得られる経験値
    char spells[MAX_SPELL_TEXT]; // 使用できる魔法（「|」区切りの文字列）

    int defend; // 防御中フラグ（1ターン）
    int evade;  // 回避中フラグ（1ターン）

    int attack_buff;        // 攻撃上昇・減少量
    int defense_buff;       // 防御上昇・減少量
    int speed_buff;         // 素早さ上昇・減少量
    int regene_heal;        // 継続回復量
    int attack_buff_turns;  // 攻撃バフ・デバフの残りターン数
    int defense_buff_turns; // 防御バフ・デバフの残りターン数
    int speed_buff_turns;   // 素早さバフ・デバフの残りターン数
    int regene_turns;       // 継続回復の残りターン数
} Character;

// 魔法1つ分のデータ
typedef struct
{
    char name[20];   // 魔法名
    int mp_cost;     // 消費MP
    int power;       // 威力（ダメージ or 回復量の基準値）
    int effect_type; // 効果タイプ（SPELL_DAMAGE など）
} Spell;

// 行動順リストの1エントリ
// 「誰が」「どのタイプか」「素早さはいくつか」をまとめる
typedef struct
{
    int index; // party[] または enemy[] 内のインデックス
    int type;  // TYPE_PLAYER or TYPE_ENEMY
    int speed; // 素早さ（ソート用）
} TurnOrder;

// ==============================
// 魔法データ一覧（定数）
// ==============================

// ゲームに存在する魔法をここにまとめて定義する
// 魔法を追加するときはここに追記してfind_spell_by_nameも更新する
static const Spell SPELL_FIRE = {"ファイア", 5, 35, SPELL_DAMAGE};
static const Spell SPELL_ICE = {"アイス", 6, 40, SPELL_DAMAGE};
static const Spell SPELL_THUNDER = {"サンダー", 8, 50, SPELL_DAMAGE};
static const Spell SPELL_HOLY = {"ホーリー", 10, 65, SPELL_DAMAGE};
static const Spell SPELL_DARK = {"ダーク", 9, 60, SPELL_DAMAGE};
static const Spell SPELL_HOIMI = {"ホイミ", 5, 40, SPELL_HEAL};
static const Spell SPELL_ZAKI = {"ザキ", 12, 50, SPELL_DEATH};
static const Spell SPELL_IORA = {"イオラ", 15, 45, SPELL_ALL_DAMAGE};
static const Spell SPELL_BAIKILT = {"バイキルト", 8, 15, SPELL_ATTACK_UP};
static const Spell SPELL_SUKURUTO = {"スクルト", 8, 15, SPELL_DEFENSE_UP};
static const Spell SPELL_PIORIM = {"ピオリム", 6, 15, SPELL_SPEED_UP};
static const Spell SPELL_DRAIN_LIFE = {"ドレイン", 12, 40, SPELL_DRAIN};
static const Spell SPELL_LUKANI = {"ルカニ", 6, 20, SPELL_DEFENSE_DOWN};   // 防御力ダウン
static const Spell SPELL_HENATOS = {"ヘナトス", 6, 20, SPELL_ATTACK_DOWN}; // 攻撃力ダウン
static const Spell SPELL_REGENE_GIFT = {"リジェネ", 8, 15, SPELL_REGENE};  // 継続回復
static const Spell SPELL_ZAORIKU = {"ザオリク", 20, 50, SPELL_REVIVE};     // 蘇生（戦闘不能から50%HPで復活）
static const Spell SPELL_MAHOTORA = {"マホトラ", 0, 15, SPELL_MP_DRAIN};   // MP吸収

// ==============================
// 関数のプロトタイプ宣言
// ==============================
// ここに書いておくと「まだ定義されていない関数を呼ぶ」エラーを防げる

void print_status(Character *ch);
void print_turn_start(int turn);
void print_actor_turn(Character *ch);

const Spell *find_spell_by_name(const char *name);
void trim_whitespace(char *text);
int get_available_spells(const Character *ch, const Spell *spells[], int max_spells);
int get_castable_spells(const Character *ch, const Spell *spells[], int max_spells);

int choose_random_alive_index(Character *characters, int count);
int choose_random_wounded_index(Character *characters, int count);

int load_characters(const char *filename, Character *party, int max_records);
void sort_order_by_speed(TurnOrder order[], int size);

void attack(Character *attacker, Character *defender);
void damage_target(Character *caster, Character *target, const Spell *spell);
void heal_target(Character *target, const Spell *spell);
void death_target(Character *target, const Spell *spell);
void buff_target(Character *target, const Spell *spell);
void update_buffs(Character *ch);
void cast_spell(Character *caster,
                Character *targets, int target_count,
                Character *allies, int ally_count,
                const Spell *spell, int selected_index);

void command(Character *actor,
             Character *targets, int target_count,
             Character *allies, int ally_count,
             int cmd);

// ==============================
// 関数の実装
// ==============================

// -------- 魔法名から魔法データを探す --------
// 見つかったらそのSpellへのポインタを返す。なければNULLを返す。
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
    if (strcmp(name, SPELL_HOIMI.name) == 0)
        return &SPELL_HOIMI;
    if (strcmp(name, SPELL_ZAKI.name) == 0)
        return &SPELL_ZAKI;
    if (strcmp(name, SPELL_IORA.name) == 0)
        return &SPELL_IORA;
    if (strcmp(name, SPELL_BAIKILT.name) == 0)
        return &SPELL_BAIKILT;
    if (strcmp(name, SPELL_SUKURUTO.name) == 0)
        return &SPELL_SUKURUTO;
    if (strcmp(name, SPELL_PIORIM.name) == 0)
        return &SPELL_PIORIM;
    if (strcmp(name, SPELL_DRAIN_LIFE.name) == 0)
        return &SPELL_DRAIN_LIFE;
    if (strcmp(name, SPELL_LUKANI.name) == 0)
        return &SPELL_LUKANI;
    if (strcmp(name, SPELL_HENATOS.name) == 0)
        return &SPELL_HENATOS;
    if (strcmp(name, SPELL_REGENE_GIFT.name) == 0)
        return &SPELL_REGENE_GIFT;
    if (strcmp(name, SPELL_ZAORIKU.name) == 0)
        return &SPELL_ZAORIKU;
    if (strcmp(name, SPELL_MAHOTORA.name) == 0)
        return &SPELL_MAHOTORA;
    return NULL; // 一致する魔法が見つからなかった
}

// -------- 文字列の前後の空白・改行を取り除く --------
// CSVを読み込んだときに余計なスペースが入ることがあるため使う
void trim_whitespace(char *text)
{
    // 先頭の空白を飛ばす
    char *start = text;
    while (*start == ' ' || *start == '\t')
        start++;

    // 先頭をずらした場合、内容を先頭に移動する
    if (start != text)
        memmove(text, start, strlen(start) + 1);

    // 末尾の空白・タブ・改行を消す
    int len = (int)strlen(text);
    while (len > 0 && (text[len - 1] == ' ' || text[len - 1] == '\t' || text[len - 1] == '\r'))
    {
        text[len - 1] = '\0';
        len--;
    }
}

// -------- キャラクターが覚えている魔法の一覧を取得する --------
// 戻り値：取得できた魔法の数
// spells[]に取得した魔法へのポインタが入る
int get_available_spells(const Character *ch, const Spell *spells[], int max_spells)
{
    // 魔法が設定されていなければ0を返す
    if (ch->spells[0] == '\0')
        return 0;

    int count = 0;

    // 元の文字列を書き換えないようにコピーしてから分割する
    char spell_text[MAX_SPELL_TEXT];
    strncpy(spell_text, ch->spells, sizeof(spell_text) - 1);
    spell_text[sizeof(spell_text) - 1] = '\0';

    // 「|」で区切って魔法名を1つずつ取り出す
    char *token = strtok(spell_text, "|");
    while (token != NULL && count < max_spells)
    {
        trim_whitespace(token);
        const Spell *spell = find_spell_by_name(token);
        if (spell != NULL)
            spells[count++] = spell;

        token = strtok(NULL, "|");
    }

    return count;
}

// -------- 今のMPで使える魔法の一覧を取得する --------
// MPが足りない魔法は除外される
int get_castable_spells(const Character *ch, const Spell *spells[], int max_spells)
{
    // まず覚えている魔法を全部取得する
    const Spell *available[MAX_SPELLS];
    int available_count = get_available_spells(ch, available, MAX_SPELLS);

    // MPが足りるものだけをspells[]に入れる
    int count = 0;
    for (int i = 0; i < available_count && count < max_spells; i++)
    {
        if (ch->mp >= available[i]->mp_cost)
            spells[count++] = available[i];
    }

    return count;
}

// -------- 生存しているキャラクターの中からランダムに1人選ぶ --------
// 戻り値：選ばれたインデックス（全員死亡なら -1）
int choose_random_alive_index(Character *characters, int count)
{
    // 生存者のインデックスを集める
    int alive_indexes[MAX_CHARACTERS];
    int alive_count = 0;

    for (int i = 0; i < count; i++)
    {
        if (characters[i].status == STATUS_ALIVE)
        {
            alive_indexes[alive_count] = i;
            alive_count++;
        }
    }

    if (alive_count == 0)
        return -1; // 全員死亡

    // その中からランダムに1人
    return alive_indexes[rand() % alive_count];
}

// -------- HPが最大より少ない（傷ついている）キャラをランダムに1人選ぶ --------
// 全員満タンの場合は生存者からランダムに選ぶ
int choose_random_wounded_index(Character *characters, int count)
{
    int wounded_indexes[MAX_CHARACTERS];
    int wounded_count = 0;

    for (int i = 0; i < count; i++)
    {
        if (characters[i].status == STATUS_ALIVE && characters[i].hp < characters[i].max_hp)
        {
            wounded_indexes[wounded_count] = i;
            wounded_count++;
        }
    }

    // 傷ついている人がいなければ生存者から選ぶ
    if (wounded_count == 0)
        return choose_random_alive_index(characters, count);

    return wounded_indexes[rand() % wounded_count];
}

// -------- CSVファイルからキャラクターを読み込む --------
// 戻り値：読み込めたキャラクター数（失敗したら -1）
int load_characters(const char *filename, Character *party, int max_records)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("ファイルを開けませんでした");
        return -1;
    }

    int count = 0;
    char line[384];

    while (fgets(line, sizeof(line), fp) && count < max_records)
    {
        // 末尾の改行を削除する
        line[strcspn(line, "\n")] = '\0';

        // '#' で始まる行はコメント行なので読み飛ばす
        if (line[0] == '#' || line[0] == '\0')
            continue;

        // 読み込む前に、構造体の全メンバーを初期化する
        memset(&party[count], 0, sizeof(Character));

        // 魔法欄を空にしてからsscanfで読む（魔法なしのキャラに対応）
        party[count].spells[0] = '\0';

        // CSV1行分をキャラクター構造体に読み込む
        int num_fields = sscanf(line,
                                "%d,%d,%19[^,],%d,%d,%d,%d,%d,%d,%d,%f,%d,%f,%f,%f,%127[^\n]",
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
                                &party[count].dodge_rate,
                                &party[count].speed,
                                &party[count].critical_rate,
                                &party[count].critical_damage,
                                &party[count].drop_exp,
                                party[count].spells);

        // 最低15項目（魔法欄は省略可）読めていれば有効な行とみなす
        if (num_fields >= 15)
        {
            trim_whitespace(party[count].spells);
            count++;
        }
    }

    fclose(fp);
    return count;
}

// -------- 行動順リストを素早さの高い順（降順）にソートする --------
// バブルソートで実装
void sort_order_by_speed(TurnOrder order[], int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            // 隣同士を比べて素早さが逆なら入れ替える
            if (order[j].speed < order[j + 1].speed)
            {
                TurnOrder temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }
}

// -------- 通常攻撃 --------
void attack(Character *attacker, Character *defender)
{
    printf("  %sの攻撃！\n", attacker->name);

    // ダメージ計算
    int attack_power = attacker->attack + attacker->attack_buff;
    int defense_power = defender->defense + defender->defense_buff;
    int damage = attack_power - defense_power;
    if (damage < 0)
        damage = 0;

    // 防御している場合はダメージ半減
    if (defender->defend)
        damage /= 2;

    // 回避判定
    float dodge = defender->dodge_rate;

    if (defender->evade)
        dodge += 0.3f;

    if ((float)rand() / RAND_MAX < dodge)
    {
        printf("  %sは回避した！\n", defender->name);
        return;
    }

    // HPを減らす（最小0）
    defender->hp -= damage;
    if (defender->hp < 0)
        defender->hp = 0;

    printf("  %sに %d ダメージ！\n", defender->name, damage);
    printf("  %sの残りHP: %d\n", defender->name, defender->hp);

    // HPが0になったら戦闘不能にする
    if (defender->hp == 0)
    {
        printf("  ★ %sは倒れた！\n", defender->name);
        defender->status = STATUS_DEAD;
    }

    printf("--------------------------------\n");
    defender->defend = 0;
    attacker->evade = 0;
}

// -------- 魔法のダメージを1体に与える（cast_spell内で使う） --------
void damage_target(Character *caster, Character *target, const Spell *spell)
{
    // ダメージ計算：魔法威力 + レベル×2 - 防御力（最小0）
    int damage = spell->power + caster->level * 2 - target->defense;
    if (damage < 0)
        damage = 0;

    // 防御している場合はダメージ半減
    if (target->defend)
        damage /= 2;

    // 回避判定
    float dodge = target->dodge_rate;
    if (target->evade)
        dodge += 0.3f;

    if ((float)rand() / RAND_MAX < dodge)
    {
        printf("  %sは回避した！\n", target->name);
        return;
    }

    target->hp -= damage;
    if (target->hp < 0)
        target->hp = 0;

    printf("  %sに %d ダメージ！\n", target->name, damage);
    printf("  %sの残りHP: %d\n", target->name, target->hp);

    if (target->hp == 0)
    {
        printf("  ★ %sは倒れた！\n", target->name);
        target->status = STATUS_DEAD;
    }
    target->defend = 0;
    caster->evade = 0;
}

// -------- 魔法でHPを回復する（cast_spell内で使う） --------
void heal_target(Character *target, const Spell *spell)
{
    int heal = spell->power;
    target->hp += heal;

    // 最大HPを超えないようにする
    if (target->hp > target->max_hp)
        target->hp = target->max_hp;

    printf("  %sのHPが %d 回復！\n", target->name, heal);
    printf("  %sの残りHP: %d\n", target->name, target->hp);
}

// -------- 即死魔法（確率でHPを0にする）（cast_spell内で使う） --------
void death_target(Character *target, const Spell *spell)
{
    // spell->power を「即死成功率（%）」として扱う
    int roll = rand() % 100;

    if (roll < spell->power)
    {
        target->hp = 0;
        target->status = STATUS_DEAD;
        printf("  %sは即死した！\n", target->name);
    }
    else
    {
        printf("  %sには効かなかった！\n", target->name);
    }
}

// -------- 継続回復（リジェネ）をかける（cast_spell内で使う） --------
void apply_regene(Character *caster, Character *target, const Spell *spell)
{
    int turns = (caster == target) ? 4 : 3;
    target->regene_heal = spell->power;
    target->regene_turns = turns;
    printf("  %sにリジェネがかかった！(毎ターン%d回復/%dターン)\n", target->name, spell->power, turns == 4 ? 3 : turns);
}

// HP吸収（ドレイン）
void drain_target(Character *caster, Character *target, const Spell *spell)
{
    int damage = spell->power + caster->level * 2 - target->defense;
    if (damage < 0)
        damage = 0;

    if (target->defend)
        damage /= 2;

    float dodge = target->dodge_rate;
    if (target->evade)
        dodge += 0.3f;

    if ((float)rand() / RAND_MAX < dodge)
    {
        printf("  %sは回避した！\n", target->name);
        return;
    }

    target->hp -= damage;
    if (target->hp < 0)
        target->hp = 0;

    printf("  %sに %d ダメージ！\n", target->name, damage);

    // 与えたダメージ分を回復
    caster->hp += damage;
    if (caster->hp > caster->max_hp)
        caster->hp = caster->max_hp;

    printf("  %sは HP を %d 吸収した！(現在HP: %d/%d)\n", caster->name, damage, caster->hp, caster->max_hp);

    if (target->hp == 0)
    {
        printf("  ★ %sは倒れた！\n", target->name);
        target->status = STATUS_DEAD;
    }
    target->defend = 0;
    caster->evade = 0;
}

// MP吸収（マホトラ）
void mp_drain_target(Character *caster, Character *target, const Spell *spell)
{
    int amount = spell->power;
    if (target->mp < amount)
        amount = target->mp; // 相手の残りMP以上に奪うことはできない

    target->mp -= amount;
    caster->mp += amount;
    if (caster->mp > caster->max_mp)
        caster->mp = caster->max_mp;

    printf("  %sのMPを %d 奪い取った！\n", target->name, amount);
    printf("  %sの残りMP: %d\n", caster->name, caster->mp);
}

// 蘇生（ザオリク）
void revive_target(Character *target, const Spell *spell)
{
    target->status = STATUS_ALIVE;
    target->hp = (target->max_hp * spell->power) / 100; // spell->powerを復活HP割合（%）とする
    if (target->hp <= 0)
        target->hp = 1;

    printf("  ★ %sが息を吹き返した！(HP: %d/%d)\n", target->name, target->hp, target->max_hp);
}

// -------- バフ魔法 --------
void apply_buff_debuff(Character *caster, Character *target, const Spell *spell)
{
    int turns = (caster == target) ? 4 : 3;

    switch (spell->effect_type)
    {
    case SPELL_ATTACK_UP:
        target->attack_buff = spell->power;
        target->attack_buff_turns = turns;
        printf("  %sの攻撃力が上がった！(%dターン)\n", target->name, turns == 4 ? 3 : turns);
        break;
    case SPELL_ATTACK_DOWN:
        target->attack_buff = -spell->power;
        target->attack_buff_turns = turns;
        printf("  %sの攻撃力が下がった！(%dターン)\n", target->name, turns == 4 ? 3 : turns);
        break;
    case SPELL_DEFENSE_UP:
        target->defense_buff = spell->power;
        target->defense_buff_turns = turns;
        printf("  %sの防御力が上がった！(%dターン)\n", target->name, turns == 4 ? 3 : turns);
        break;
    case SPELL_DEFENSE_DOWN:
        target->defense_buff = -spell->power;
        target->defense_buff_turns = turns;
        printf("  %sの防御力が下がった！(%dターン)\n", target->name, turns == 4 ? 3 : turns);
        break;
    case SPELL_SPEED_UP:
        target->speed_buff = spell->power;
        target->speed_buff_turns = turns;
        printf("  %sの素早さが上がった！(%dターン)\n", target->name, turns == 4 ? 3 : turns);
        break;
    }
}

void update_buffs(Character *ch)
{
    // 攻撃バフ・デバフ
    if (ch->attack_buff_turns > 0)
    {
        ch->attack_buff_turns--;
        if (ch->attack_buff_turns == 0)
        {
            if (ch->attack_buff > 0)
                printf("  %sの攻撃力上昇が切れた！\n", ch->name);
            else if (ch->attack_buff < 0)
                printf("  %sの攻撃力低下が元に戻った！\n", ch->name);
            ch->attack_buff = 0;
        }
    }

    // 防御バフ・デバフ
    if (ch->defense_buff_turns > 0)
    {
        ch->defense_buff_turns--;
        if (ch->defense_buff_turns == 0)
        {
            if (ch->defense_buff > 0)
                printf("  %sの防御力上昇が切れた！\n", ch->name);
            else if (ch->defense_buff < 0)
                printf("  %sの防御力低下が元に戻った！\n", ch->name);
            ch->defense_buff = 0;
        }
    }

    // 素早さバフ・デバフ
    if (ch->speed_buff_turns > 0)
    {
        ch->speed_buff_turns--;
        if (ch->speed_buff_turns == 0)
        {
            if (ch->speed_buff > 0)
                printf("  %sの素早さ上昇が切れた！\n", ch->name);
            else if (ch->speed_buff < 0)
                printf("  %sの素早さ低下が元に戻った！\n", ch->name);
            ch->speed_buff = 0;
        }
    }

    // 継続回復
    if (ch->regene_turns > 0)
    {
        ch->regene_turns--;
        if (ch->status == STATUS_ALIVE)
        {
            ch->hp += ch->regene_heal;
            if (ch->hp > ch->max_hp)
                ch->hp = ch->max_hp;

            printf("  %sはリジェネの効果で HP が %d 回復した！(残り%dターン)\n", ch->name, ch->regene_heal, ch->regene_turns);

            if (ch->regene_turns == 0)
            {
                printf("  %sのリジェネ効果が切れた！\n", ch->name);
                ch->regene_heal = 0;
            }
        }
        else
        {
            // 戦闘不能になったらリジェネはクリア
            ch->regene_turns = 0;
            ch->regene_heal = 0;
        }
    }
}

// -------- 魔法を詠唱する --------
// selected_index：対象のインデックス（全体魔法のときは -1）
void cast_spell(Character *caster,
                Character *targets, int target_count,
                Character *allies, int ally_count,
                const Spell *spell, int selected_index)
{
    if (caster->mp < spell->mp_cost)
    {
        printf("MPが足りません\n");
        printf("--------------------------------\n");
        return;
    }

    printf("  %sは%sを唱えた！\n", caster->name, spell->name);
    caster->mp -= spell->mp_cost;

    // 【修正・追加】効果タイプによる処理の分岐
    if (spell->effect_type == SPELL_HEAL)
    {
        if (selected_index < 0 || selected_index >= ally_count || allies[selected_index].status != STATUS_ALIVE)
            printf("対象がいません\n");
        else
            heal_target(&allies[selected_index], spell);
    }
    else if (spell->effect_type == SPELL_REVIVE)
    {
        // 蘇生魔法：戦闘不能の味方が対象
        if (selected_index < 0 || selected_index >= ally_count || allies[selected_index].status != STATUS_DEAD)
            printf("対象がいないか、またはまだ生存しています\n");
        else
            revive_target(&allies[selected_index], spell);
    }
    else if (spell->effect_type == SPELL_REGENE)
    {
        if (selected_index < 0 || selected_index >= ally_count || allies[selected_index].status != STATUS_ALIVE)
            printf("対象がいません\n");
        else
            apply_regene(caster, &allies[selected_index], spell);
    }
    else if (spell->effect_type == SPELL_ATTACK_UP || spell->effect_type == SPELL_DEFENSE_UP || spell->effect_type == SPELL_SPEED_UP)
    {
        if (selected_index < 0 || selected_index >= ally_count || allies[selected_index].status != STATUS_ALIVE)
            printf("対象がいません\n");
        else
            apply_buff_debuff(caster, &allies[selected_index], spell);
    }
    else if (spell->effect_type == SPELL_ATTACK_DOWN || spell->effect_type == SPELL_DEFENSE_DOWN)
    {
        // 敵へのデバフ魔法
        if (selected_index < 0 || selected_index >= target_count || targets[selected_index].status != STATUS_ALIVE)
            printf("対象がいません\n");
        else
            apply_buff_debuff(caster, &targets[selected_index], spell);
    }
    else if (spell->effect_type == SPELL_DRAIN)
    {
        if (selected_index < 0 || selected_index >= target_count || targets[selected_index].status != STATUS_ALIVE)
            printf("対象がいません\n");
        else
            drain_target(caster, &targets[selected_index], spell);
    }
    else if (spell->effect_type == SPELL_MP_DRAIN)
    {
        if (selected_index < 0 || selected_index >= target_count || targets[selected_index].status != STATUS_ALIVE)
            printf("対象がいません\n");
        else
            mp_drain_target(caster, &targets[selected_index], spell);
    }
    else if (spell->effect_type == SPELL_DEATH)
    {
        if (selected_index < 0 || selected_index >= target_count || targets[selected_index].status != STATUS_ALIVE)
            printf("対象がいません\n");
        else
            death_target(&targets[selected_index], spell);
    }
    else if (spell->effect_type == SPELL_ALL_DAMAGE)
    {
        for (int i = 0; i < target_count; i++)
        {
            if (targets[i].status == STATUS_ALIVE)
                damage_target(caster, &targets[i], spell);
        }
    }
    else
    {
        if (selected_index < 0 || selected_index >= target_count || targets[selected_index].status != STATUS_ALIVE)
            printf("対象がいません\n");
        else
            damage_target(caster, &targets[selected_index], spell);
    }

    printf("  %sの残りMP: %d\n", caster->name, caster->mp);
    printf("--------------------------------\n");
}

// -------- コマンドを実行する（プレイヤー・エネミー共通） --------
void command(Character *actor,
             Character *targets, int target_count,
             Character *allies, int ally_count,
             int cmd)
{
    int target_index;

    switch (cmd)
    {
    // ===== 0: 情報確認 =====
    case 0:
    {
        int next_cmd;

        // 自分のステータスと敵の一覧を表示する
        print_status(actor);
        printf("生存している敵:\n");
        for (int i = 0; i < target_count; i++)
        {
            if (targets[i].status == STATUS_ALIVE)
                printf("  %d: %s (HP: %d)\n", i, targets[i].name, targets[i].hp);
        }
        printf("--------------------------------\n");

        // コマンドを再入力してもらう
        printf("  コマンドを選択してください:\n");
        printf("  0: 情報確認 | 1: 攻撃 | 2: 魔法 | 3: 防御 | 4: 回避\n");
        printf("  選択: ");
        scanf("%d", &next_cmd);
        command(actor, targets, target_count, allies, ally_count, next_cmd);
        break;
    }

    // ===== 1: 攻撃 =====
    case 1:
        if (actor->type == TYPE_PLAYER)
        {
            // プレイヤーは攻撃対象を自分で選ぶ
            printf("攻撃対象を選択してください:\n");
            for (int i = 0; i < target_count; i++)
            {
                if (targets[i].status == STATUS_ALIVE)
                    printf("  %d: %s (HP: %d)\n", i, targets[i].name, targets[i].hp);
            }
            printf("選択: ");
            scanf("%d", &target_index);

            // 無効な入力なら選び直し
            if (target_index < 0 || target_index >= target_count || targets[target_index].status != STATUS_ALIVE)
            {
                printf("無効な選択です\n");
                command(actor, targets, target_count, allies, ally_count, cmd);
                return;
            }
            attack(actor, &targets[target_index]);
        }
        else
        {
            // エネミーはランダムに対象を選ぶ
            target_index = choose_random_alive_index(targets, target_count);
            if (target_index < 0)
                return; // 攻撃できる相手がいない

            attack(actor, &targets[target_index]);
        }
        break;

        // ===== 2: 魔法 =====
    case 2:
        if (actor->type == TYPE_PLAYER)
        {
            const Spell *spells[MAX_SPELLS];
            int spell_count = get_available_spells(actor, spells, MAX_SPELLS);
            int spell_index;
            int next_cmd;

            if (spell_count == 0)
            {
                printf("%sは魔法を覚えていません\n", actor->name);
                printf("--------------------------------\n");
                printf("  コマンドを選択してください:\n");
                printf("  0: 情報確認 | 1: 攻撃 | 2: 魔法 | 3: 防御 | 4: 回避\n");
                printf("  選択: ");
                scanf("%d", &next_cmd);
                command(actor, targets, target_count, allies, ally_count, next_cmd);
                break;
            }

            printf("使用する魔法を選択してください:\n");
            for (int i = 0; i < spell_count; i++)
                printf("  %d: %s (消費MP: %d)\n", i, spells[i]->name, spells[i]->mp_cost);

            printf("選択: ");
            scanf("%d", &spell_index);

            if (spell_index < 0 || spell_index >= spell_count)
            {
                printf("無効な選択です\n");
                command(actor, targets, target_count, allies, ally_count, cmd);
                return;
            }

            // 魔法タイプに応じて表示するステータスを切り替えて対象を選ぶ
            if (spells[spell_index]->effect_type == SPELL_HEAL ||
                spells[spell_index]->effect_type == SPELL_ATTACK_UP ||
                spells[spell_index]->effect_type == SPELL_DEFENSE_UP ||
                spells[spell_index]->effect_type == SPELL_SPEED_UP ||
                spells[spell_index]->effect_type == SPELL_REGENE)
            {
                // 生存している味方から対象を選ぶ
                printf("対象を選択してください:\n");
                for (int i = 0; i < ally_count; i++)
                {
                    if (allies[i].status == STATUS_ALIVE)
                    {
                        switch (spells[spell_index]->effect_type)
                        {
                        case SPELL_HEAL:
                            printf("  %d: %s (HP: %d/%d)\n", i, allies[i].name, allies[i].hp, allies[i].max_hp);
                            break;
                        case SPELL_ATTACK_UP:
                            printf("  %d: %s (攻撃力: %d)\n", i, allies[i].name, allies[i].attack + allies[i].attack_buff);
                            break;
                        case SPELL_DEFENSE_UP:
                            printf("  %d: %s (防御力: %d)\n", i, allies[i].name, allies[i].defense + allies[i].defense_buff);
                            break;
                        case SPELL_SPEED_UP:
                            printf("  %d: %s (素早さ: %d)\n", i, allies[i].name, allies[i].speed + allies[i].speed_buff);
                            break;
                        case SPELL_REGENE:
                            printf("  %d: %s (HP: %d/%d, リジェネ回復量: +%d/T)\n", i, allies[i].name, allies[i].hp, allies[i].max_hp, spells[spell_index]->power);
                            break;
                        }
                    }
                }
                printf("選択: ");
                scanf("%d", &target_index);

                if (target_index < 0 || target_index >= ally_count || allies[target_index].status != STATUS_ALIVE)
                {
                    printf("無効な選択です\n");
                    command(actor, targets, target_count, allies, ally_count, cmd);
                    return;
                }
            }
            else if (spells[spell_index]->effect_type == SPELL_REVIVE)
            {
                // 蘇生魔法：死亡している味方から対象を選ぶ
                printf("生き返らせる対象を選択してください:\n");
                int dead_count = 0;
                for (int i = 0; i < ally_count; i++)
                {
                    if (allies[i].status == STATUS_DEAD)
                    {
                        printf("  %d: %s\n", i, allies[i].name);
                        dead_count++;
                    }
                }
                if (dead_count == 0)
                {
                    printf("蘇生できる対象がいません\n");
                    command(actor, targets, target_count, allies, ally_count, cmd);
                    return;
                }
                printf("選択: ");
                scanf("%d", &target_index);

                if (target_index < 0 || target_index >= ally_count || allies[target_index].status != STATUS_DEAD)
                {
                    printf("無効な選択です\n");
                    command(actor, targets, target_count, allies, ally_count, cmd);
                    return;
                }
            }
            else if (spells[spell_index]->effect_type == SPELL_ALL_DAMAGE)
            {
                target_index = -1; // 全体魔法
            }
            else
            {
                // 敵を対象とする魔法（単体ダメージ、デバフ、ドレイン、MP吸収、即死）
                printf("魔法の対象を選択してください:\n");
                for (int i = 0; i < target_count; i++)
                {
                    if (targets[i].status == STATUS_ALIVE)
                    {
                        switch (spells[spell_index]->effect_type)
                        {
                        case SPELL_ATTACK_DOWN:
                            printf("  %d: %s (攻撃力: %d)\n", i, targets[i].name, targets[i].attack + targets[i].attack_buff);
                            break;
                        case SPELL_DEFENSE_DOWN:
                            printf("  %d: %s (防御力: %d)\n", i, targets[i].name, targets[i].defense + targets[i].defense_buff);
                            break;
                        default:
                            printf("  %d: %s (HP: %d)\n", i, targets[i].name, targets[i].hp);
                            break;
                        }
                    }
                }
                printf("選択: ");
                scanf("%d", &target_index);

                if (target_index < 0 || target_index >= target_count || targets[target_index].status != STATUS_ALIVE)
                {
                    printf("無効な選択です\n");
                    command(actor, targets, target_count, allies, ally_count, cmd);
                    return;
                }
            }

            cast_spell(actor, targets, target_count, allies, ally_count, spells[spell_index], target_index);
        }
        else
        {
            // エネミーの魔法自動処理
            const Spell *spells[MAX_SPELLS];
            int spell_count = get_castable_spells(actor, spells, MAX_SPELLS);

            if (spell_count == 0)
            {
                target_index = choose_random_alive_index(targets, target_count);
                if (target_index < 0)
                    return;
                attack(actor, &targets[target_index]);
                break;
            }

            const Spell *spell = spells[rand() % spell_count];

            if (spell->effect_type == SPELL_HEAL || spell->effect_type == SPELL_REGENE)
            {
                target_index = choose_random_wounded_index(allies, ally_count);
            }
            else if (spell->effect_type == SPELL_ATTACK_UP || spell->effect_type == SPELL_DEFENSE_UP || spell->effect_type == SPELL_SPEED_UP)
            {
                target_index = choose_random_alive_index(allies, ally_count);
            }
            else if (spell->effect_type == SPELL_REVIVE)
            {
                // 死亡している味方エネミーを探す
                int dead_indexes[MAX_CHARACTERS];
                int dead_count = 0;
                for (int j = 0; j < ally_count; j++)
                {
                    if (allies[j].status == STATUS_DEAD)
                        dead_indexes[dead_count++] = j;
                }
                target_index = (dead_count > 0) ? dead_indexes[rand() % dead_count] : -1;
            }
            else if (spell->effect_type == SPELL_ALL_DAMAGE)
            {
                target_index = -1;
            }
            else
            {
                target_index = choose_random_alive_index(targets, target_count);
            }

            // 蘇生魔法以外で対象が見つからない場合は通常攻撃
            if (target_index < 0 && spell->effect_type != SPELL_ALL_DAMAGE)
            {
                target_index = choose_random_alive_index(targets, target_count);
                if (target_index >= 0)
                    attack(actor, &targets[target_index]);
                break;
            }

            cast_spell(actor, targets, target_count, allies, ally_count, spell, target_index);
        }
        break;

    // ===== 3: 防御（未実装） =====
    case 3:
        actor->defend = 1;

        printf("%sは防御に集中！\n", actor->name);

        printf("次の被ダメージ半減！\n");
        break;

    // ===== 4: 回避（未実装） =====
    case 4:
        actor->evade = 1;

        printf("%sは回避に集中！\n", actor->name);

        printf("回避率アップ！\n");
        break;

    // ===== 無効な入力 =====
    default:
        printf("無効なコマンドです\n");
        command(actor, targets, target_count, allies, ally_count, cmd);
        break;
    }
}

// ==============================
// 表示系の関数
// ==============================

// キャラクターのステータスを表示する
void print_status(Character *ch)
{
    printf("【%s】のステータス:\n", ch->name);
    printf("  HP: %d/%d\n", ch->hp, ch->max_hp);
    printf("  MP: %d/%d\n", ch->mp, ch->max_mp);
    printf("  攻撃力:   %d\n", ch->attack);
    printf("  防御力:   %d\n", ch->defense);
    printf("  素早さ:   %d\n", ch->speed);
    printf("  クリティカル率:   %.2f\n", ch->critical_rate);
    printf("  クリティカル倍率: %.1f\n", ch->critical_damage);
    printf("  魔法: %s\n", ch->spells[0] == '\0' ? "なし" : ch->spells);
    printf("--------------------------------\n");
}

// ターン開始を表示する
void print_turn_start(int turn)
{
    printf("\n==============================\n");
    printf("◆ %dターン目 開始\n", turn);
    printf("==============================\n\n");
}

// 誰のターンかを表示する
void print_actor_turn(Character *ch)
{
    printf(">【%s】のターン（HP: %d）\n", ch->name, ch->hp);
}

void lvl_up(Character *ch)
{
    ch->level++;
    ch->max_hp += 10;
    ch->hp = ch->max_hp; // レベルアップでHP全回復
    ch->max_mp += 5;
    ch->mp = ch->max_mp; // レベルアップでMP全回復
    ch->attack += 2;
    ch->defense += 2;
    ch->speed += 1;

    printf("%sはレベルアップ！ レベル%dになった！\n", ch->name, ch->level);
}

void gain_exp(Character *ch, float exp)
{
    printf("%sは%.1fの経験値を得た！\n", ch->name, exp);
    ch->drop_exp += exp;

    // 経験値が10以上ならレベルアップ
    while (ch->drop_exp >= 10 + ch->level * 2)
    {
        ch->drop_exp -= 10 + ch->level * 2; // レベルに応じて必要経験値が増える
        lvl_up(ch);
    }
}

void save_data(const Character *party, int count, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        perror("ファイルを開けませんでした");
        return;
    }

    // ヘッダー行
    fprintf(fp, "status,type,name,level,hp,max_hp,mp,max_mp,attack,defense,dodge_rate,speed,critical_rate,critical_damage,drop_exp,spells\n");
    for (int i = 0; i < count; i++)
    {
        fprintf(fp,
                "%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%f,%d,%f,%f,%f,%s\n",
                party[i].status,
                party[i].type,
                party[i].name,
                party[i].level,
                party[i].hp,
                party[i].max_hp,
                party[i].mp,
                party[i].max_mp,
                party[i].attack,
                party[i].defense,
                party[i].dodge_rate,
                party[i].speed,
                party[i].critical_rate,
                party[i].critical_damage,
                party[i].drop_exp,
                party[i].spells);
    }

    fclose(fp);
}

#endif /* RPG_H */