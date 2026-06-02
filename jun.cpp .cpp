 // Game project for college assignment
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <Windows.h>

// 1. Базовый класс для головоломок
class Puzzle {
public:
    virtual ~Puzzle() = default;
    virtual bool solve() = 0; // Возвращает true, если головоломка решена
};

// 2. Головоломка 1: Математическая загадка
class MathPuzzle : public Puzzle {
public:
    bool solve() override {
        std::cout << "\n--- ГОЛОВОЛОМКА: Древний механизм ---\n";
        std::cout << "На стене высечено: 'Умножь количество пальцев на руке на число текстовых локаций в этом задании (3) и прибавь 5'.\n";
        std::cout << "Введите ответ: ";
        int answer;
        if (std::cin >> answer && answer == 20) {
            std::cout << "Раздался щелчок. Дверь открылась!\n";
            return true;
        }
        std::cout << "Неверно. Механизм заклинило, попробуйте еще раз.\n";
        return false;
    }
};

// 3. Головоломка 2: Анаграмма (Словесная)
class WordPuzzle : public Puzzle {
public:
    bool solve() override {
        std::cout << "\n--- ГОЛОВОЛОМКА: Загадка Сфинкса ---\n";
        std::cout << "Расшифруйте анаграмму слова, обозначающего выход: 'ХОВД'\n";
        std::cout << "Введите слово ЗАГЛАВНЫМИ буквами: ";
        std::string answer;
        std::cin >> answer;
        if (answer == "ВХОД") {
            std::cout << "Стена с грохотом отодвигается!\n";
            return true;
        }
        std::cout << "Эхо повторяет ваш неверный ответ... Попробуйте снова.\n";
        return false;
    }
};

// 4. Сущность Локации
struct Location {
    int id;                                 // Уникальный номер локации
    std::string name;                       // Название
    std::vector<std::string> form;          // Массив строк для ASCII отрисовки
    std::vector<int> connectedLocations;    // Связи (ID локаций, куда можно перейти)
    bool isOpen;                            // Состояние: открыта или закрыта
    std::shared_ptr<Puzzle> puzzle;         // Указатель на головоломку (NULL если открыта)

    void draw() const {
        std::cout << "\n=========================================\n";
        std::cout << " Локация: " << name << "\n";
        std::cout << "=========================================\n";
        for (const auto& line : form) {
            std::cout << line << "\n";
        }
        std::cout << "=========================================\n";
    }
}; // <-- Точка с запятой здесь критически важна! Если её стереть, следующий класс подчеркнется красным.

// 5. Сущность Персонажа
class Character {
private:
    int current_location_id; // ID локации, где находится персонаж

public:
    Character(int start_location_id) : current_location_id(start_location_id) {}

    int getCurrentLocationId() const {
        return current_location_id;
    }

    void moveTo(int new_location_id) {
        current_location_id = new_location_id;
    }
};

// 6. Класс управления игрой (Движок квеста)
class GameEngine {
private:
    std::vector<Location> locations;
    Character player;

    void initGame() {
        // Первая локация (Старт)
        Location loc1;
        loc1.id = 1;
        loc1.name = "Сырая Келья (Старт)";
        loc1.isOpen = true;
        loc1.puzzle = nullptr;
        loc1.connectedLocations = { 2 }; 
        loc1.form = {
            "  +-----------------+  ",
            "  |  [ ]       [ ]  |  ",
            "  |      (o_o)      |  ",
            "  |       /|\\       |  ",
            "  |       / \\       |  ",
            "  +------======-----+  "
        };

        // Промежуточная локация (Изначально закрыта)
        Location loc2;
        loc2.id = 2;
        loc2.name = "Зал Древних Механизмов";
        loc2.isOpen = false;
        loc2.puzzle = std::make_shared<MathPuzzle>();
        loc2.connectedLocations = { 1, 3 }; 
        loc2.form = {
            "  +------======-----+  ",
            "  |  /\\         /\\  |  ",
            "  | /  \\       /  \\ |  ",
            "  |      [???]      |  ",
            "  |                 |  ",
            "  +------======-----+  "
        };

        // Последняя локация (Победа, изначально закрыта)
        Location loc3;
        loc3.id = 3;
        loc3.name = "Портал Свободы (Выход)";
        loc3.isOpen = false;
        loc3.puzzle = std::make_shared<WordPuzzle>();
        loc3.connectedLocations = { 2 }; 
        loc3.form = {
            "  +------======-----+  ",
            "  |     _  _  _     |  ",
            "  |    | || || |    |  ",
            "  |    |        |   |  ",
            "  |   _|_ _ _ _|_   |  ",
            "  +-----------------+  "
        };

        locations.push_back(loc1);
        locations.push_back(loc2);
        locations.push_back(loc3);
    }

    Location* findLocationById(int id) {
        for (auto& loc : locations) {
            if (loc.id == id) return &loc;
        }
        return nullptr;
    }

public:
    GameEngine() : player(1) {
        initGame();
    }

    void run() {
        std::cout << "ДОБРО ПОЖАЛОВАТЬ В ПОДЗЕМНЫЙ ЛАБИРИНТ!\n";
        std::cout << "Ваша задача — выбраться на свободу.\n";

        while (true) {
            Location* currentLoc = findLocationById(player.getCurrentLocationId());
            if (!currentLoc) break;

            currentLoc->draw();

            if (currentLoc->id == 3 && currentLoc->isOpen) {
                std::cout << "\n🎉 ПОЗДРАВЛЯЕМ! Вы открыли последний портал и выбрались из лабиринта! 🎉\n";
                break;
            }

            std::cout << "Доступные переходы и двери вокруг вас:\n";
            for (size_t i = 0; i < currentLoc->connectedLocations.size(); ++i) {
                int nextId = currentLoc->connectedLocations[i];
                Location* nextLoc = findLocationById(nextId);
                if (nextLoc) {
                    std::cout << i + 1 << ". Направиться к: " << nextLoc->name;
                    if (!nextLoc->isOpen) {
                        std::cout << " [ЗАКРЫТО — ТРЕБУЕТСЯ РЕШИТЬ ГОЛОВОЛОМКУ]";
                    } else {
                        std::cout << " [ОТКРЫТО]";
                    }
                    std::cout << "\n";
                }
            }
            std::cout << "0. Выйти из игры\n";
            std::cout << "Выберите действие: ";

            int choice;
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Неверный ввод. Выберите число.\n";
                continue;
            }

            if (choice == 0) {
                std::cout << "Вы сдались и остались в лабиринте...\n";
                break;
            }

            if (choice > 0 && choice <= static_cast<int>(currentLoc->connectedLocations.size())) {
                int targetId = currentLoc->connectedLocations[choice - 1];
                Location* targetLoc = findLocationById(targetId);

                if (targetLoc) {
                    if (!targetLoc->isOpen) {
                        std::cout << "\nПуть заблокирован. Чтобы открыть локацию '" << targetLoc->name << "', решите загадку:\n";
                        if (targetLoc->puzzle && targetLoc->puzzle->solve()) {
                            targetLoc->isOpen = true;
                            targetLoc->puzzle = nullptr; 
                            player.moveTo(targetLoc->id); 
                        }
                    } else {
                        player.moveTo(targetLoc->id);
                        std::cout << "Вы перешли в локацию: " << targetLoc->name << "\n";
                    }
                }
            } else {
                std::cout << "Неверный выбор. Попробуйте снова.\n";
            }
        }
    }
};

// 7. Точка входа
int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    GameEngine game;
    game.run();

    std::cout << "\nНажмите Enter для завершения программы...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
    return 0;
}
