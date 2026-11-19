#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

class Damageable
{
public:
    virtual ~Damageable() = default;
    virtual void takeDamage(int damage) = 0;
    virtual bool isDestroyed() const = 0;
};

class Hero
{
private:
    string name;
    int hp;
    int maxHp;
    int gold;
    int keys;
    int power;

public:
    Hero(const string& name, int hp, int maxHp, int power)
        : name(name), hp(hp), maxHp(maxHp), gold(0), keys(0), power(power)
    {
    }

    string getName() const
    {
        return name;
    }

    void addGold(int amount)
    {
        gold += amount;
    }

    bool spendGold(int amount)
    {
        if (amount > gold)
            return false;

        gold -= amount;
        return true;
    }

    void addKey()
    {
        keys++;
    }

    bool useKey()
    {
        if (keys <= 0)
            return false;

        keys--;
        return true;
    }

    void heal(int amount)
    {
        hp += amount;

        if (hp > maxHp)
            hp = maxHp;
    }

    void attack(Damageable& target)
    {
        cout << name << " б'є: " << power << " шкоди" << endl;
        target.takeDamage(power);
    }

    void print() const
    {
        cout << name << ": hp " << hp << "/" << maxHp << ", золото " << gold << ", ключів " << keys << endl;
    }
};

class Interactable
{
public:
    virtual ~Interactable() = default;
    virtual string describe() const = 0;
    virtual void interact(Hero& hero) = 0;
};

class Door : public Interactable, public Damageable
{
private:
    string name;
    bool locked;
    int durability;

public:
    Door(const string& name, bool locked, int durability)
        : name(name), locked(locked), durability(durability)
    {
    }

    string describe() const override
    {
        if (locked)
            return name + " (зачинені)";

        return name + " (відчинені)";
    }

    void interact(Hero& hero) override
    {
        if (!locked)
        {
            cout << "Ви проходите далі" << endl;
        }
        else if (hero.useKey())
        {
            locked = false;
            cout << "Ключ підійшов! " << name << " відчинено" << endl;
        }
        else
        {
            cout << "Зачинено. Потрібен ключ" << endl;
        }
    }

    void takeDamage(int damage) override
    {
        if (durability == 0)
            return;

        durability -= damage;

        if (durability < 0)
            durability = 0;

        if (durability == 0)
        {
            locked = false;
            cout << name << " виламано!" << endl;
        }
    }

    bool isDestroyed() const override
    {
        return durability == 0;
    }
};

class Chest : public Interactable
{
private:
    int gold;
    bool hasKeyInside;
    bool opened;

public:
    Chest(int gold, bool hasKeyInside)
        : gold(gold), hasKeyInside(hasKeyInside), opened(false)
    {
    }

    string describe() const override
    {
        if (opened)
            return "Скриня (порожня)";

        return "Скриня";
    }

    void interact(Hero& hero) override
    {
        if (opened)
        {
            cout << "Скриня порожня" << endl;
            return;
        }

        opened = true;
        hero.addGold(gold);
        cout << "Ви знайшли " << gold << " золота";

        if (hasKeyInside)
        {
            hero.addKey();
            cout << " і ключ";
        }

        cout << endl;
    }
};

class Fountain : public Interactable
{
private:
    int healAmount;

public:
    explicit Fountain(int healAmount) : healAmount(healAmount)
    {
    }

    string describe() const override
    {
        return "Фонтан зцілення";
    }

    void interact(Hero& hero) override
    {
        hero.heal(healAmount);
        cout << "Ви п'єте з фонтану: +" << healAmount << " hp" << endl;
    }
};

void use(Hero& hero, Interactable& object)
{
    cout << "> " << object.describe() << endl;
    object.interact(hero);
    hero.print();
}

class Npc : public Interactable
{
protected:
    string name;
    string phrase;

public:
    Npc(const string& name, const string& phrase) : name(name), phrase(phrase)
    {
    }

    string describe() const override
    {
        return name;
    }

    void greet() const
    {
        cout << name << ": \"" << phrase << "\"" << endl;
    }
};

class Merchant : public Npc
{
private:
    int potionPrice;
    int potionHeal;

public:
    Merchant(const string& name, const string& phrase, int potionPrice, int potionHeal)
        : Npc(name, phrase), potionPrice(potionPrice), potionHeal(potionHeal)
    {
    }

    void interact(Hero& hero) override
    {
        greet();

        if (hero.spendGold(potionPrice))
        {
            hero.heal(potionHeal);
            cout << "Ви купили зілля: +" << potionHeal << " hp" << endl;
        }
        else
        {
            cout << "Не вистачає золота" << endl;
        }
    }
};

class Healer : public Npc
{
private:
    bool helped;

public:
    Healer(const string& name, const string& phrase) : Npc(name, phrase), helped(false)
    {
    }

    void interact(Hero& hero) override
    {
        greet();

        if (!helped)
        {
            helped = true;
            hero.heal(50);
            cout << "Цілителька лікує вас: +50 hp" << endl;
        }
        else
        {
            cout << "Я вже допомогла тобі сьогодні" << endl;
        }
    }
};

class Barrel : public Damageable
{
private:
    int durability;

public:
    explicit Barrel(int durability) : durability(durability)
    {
    }

    void takeDamage(int damage) override
    {
        if (durability == 0)
            return;

        durability -= damage;

        if (durability < 0)
            durability = 0;

        if (durability == 0)
            cout << "Бочку розбито" << endl;
    }

    bool isDestroyed() const override
    {
        return durability == 0;
    }
};

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    Hero conan{ "Конан", 60, 100, 10 };
    Door oakDoor{ "Дубові двері", true, 50 };
    Chest chest{ 30, true };
    Fountain fountain{ 25 };

    use(conan, oakDoor);
    use(conan, chest);
    use(conan, oakDoor);
    use(conan, chest);
    use(conan, fountain);

    cout << endl;

    Hero lira{ "Ліра", 30, 100, 10 };

    Interactable** room = new Interactable * [5];
    room[0] = new Chest(20, false);
    room[1] = new Merchant("Гном Борін", "Найкращі зілля в підземеллі!", 15, 40);
    room[2] = new Healer("Сестра Мирна", "Світло з тобою, подорожній");
    room[3] = new Merchant("Гном Борін", "Найкращі зілля в підземеллі!", 15, 40);
    room[4] = new Fountain(10);

    for (int i = 0; i < 5; i++)
    {
        use(lira, *room[i]);
    }

    for (int i = 0; i < 5; i++)
    {
        delete room[i];
    }
    delete[] room;

    cout << endl;

    Hero grim{ "Ґрім", 80, 100, 10 };
    Door ironDoor{ "Залізні двері", true, 30 };
    Barrel barrel{ 15 };

    use(grim, ironDoor);

    while (!ironDoor.isDestroyed())
    {
        grim.attack(ironDoor);
    }

    use(grim, ironDoor);

    while (!barrel.isDestroyed())
    {
        grim.attack(barrel);
    }

    Interactable* asObject = &ironDoor;
    Damageable* asTarget = &ironDoor;

    cout << "Як Interactable: " << asObject->describe() << endl;
    cout << "Як Damageable: зламані - " << (asTarget->isDestroyed() ? "так" : "ні") << endl;

    return 0;
}

/*
ПОЯСНИ

1. use потребує від об'єкта лише describe() та interact(). Усі три класи реалізують Interactable, тому
   вони це вміють. Яка саме це річ, use не знає і їй байдуже: виклик сам іде в потрібну версію методу.

2. Спільна база з name і hp змусила б фонтан мати hp, а скриню - name, хоча їм це не потрібно.
   Інтерфейс не нав'язує ні полів, ні будови: він каже лише "це вміє взаємодіяти".

3. Інтерфейс - тільки обіцянка: самі чисто віртуальні методи, ні полів, ні готового коду.
   Npc - напівготова основа: поля name і phrase, describe() і greet() однакові для всіх NPC,
   тому вони написані один раз. А interact у кожного NPC свій, тому він лишається чисто віртуальним.

4. Об'єкт Npc створити не можна, бо в нього є метод без реалізації (interact), і було б незрозуміло,
   що він має робити. Якщо Healer забуде перевизначити interact, він теж стане абстрактним,
   і new Healer(...) не скомпілюється.

5. Нуль рядків. use працює з Interactable&, а торговець і цілителька теж Interactable,
   тому підходять без змін.

6. Об'єкти видаляються через delete room[i], а room[i] має тип Interactable*. Без віртуального
   деструктора викликався б лише деструктор інтерфейсу, а деструктори Merchant і Healer ні: у них є
   свої поля (рядки), які б не очистилися. У самого інтерфейсу полів немає, але вони є в нащадків.

7. Через asObject доступні describe() та interact(). Через asTarget - takeDamage() та isDestroyed().
   Проблеми ромба немає, бо в інтерфейсів немає полів і спільного предка: нема чого дублювати,
   тому Door не отримує двох копій одних і тих самих даних.
*/