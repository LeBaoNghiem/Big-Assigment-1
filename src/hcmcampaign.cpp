#include "hcmcampaign.h"
//!----------------------------------------------
//! Lớp Position
//!----------------------------------------------
Position::Position(int r, int c) : r(r), c(c) {}

Position::Position(const string &str_pos) {
    string s = str_pos;
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == '\"' || s[i] == '(' || s[i] == ',' || s[i] == ')' ||
            s[i] == '\"') {
            s[i] = ' ';
        }
    }

    stringstream ss(s);
    ss >> r >> c;
}

int Position::getRow() const { return r; }

int Position::getCol() const { return c; }

void Position::setRow(int r) { this->r = std::max(0, r); }
void Position::setCol(int c) { this->c = std::max(0, c); }

string Position::str() const {
    stringstream ss;
    ss << "(" << r << "," << c << ")";
    return ss.str();
}

//!----------------------------------------------
//! Lớp Unit
//!----------------------------------------------
Unit::Unit(int quantity, int weight, Position pos) : pos(pos) {
    this->quantity = max(1, quantity);
    this->weight = max(1, weight);
}

Unit::~Unit() {}
Position Unit::getCurrentPosition() const { return pos; }

//!----------------------------------------------
//! Lớp Infantry
//!----------------------------------------------
Infantry::Infantry(int quantity, int weight, Position pos,
                   InfantryType infantryType)
    : Unit(quantity, weight, pos), infantryType(infantryType) {
    attackScore = 0;
}

int Infantry::getAttackScore() {
    int score = caculateScore(infantryType, quantity, weight);

    if (infantryType == 4 && isPerfectSquare(weight)) {
        score += 75;
    }

    long long temp = score * 10000 + 1975;
    if (temp > INT_MAX)
        temp = INT_MAX;
    score = updateScore((int)temp);

    if (score > 7)
        quantity = CommonFunction::getCorrectRounded(quantity + quantity * 0.2);
    else if (score < 3) {
        quantity = CommonFunction::getCorrectRounded(quantity - quantity * 0.1);
        quantity = max(1, quantity);
    }

    return caculateScore(infantryType, quantity, weight);
}

string Infantry::str() const {
    ostringstream oss;
    oss << "Infantry[" << "infantryType=" << infantryTypeToString(infantryType)
        << ","
        << "quantity=" << quantity << ","
        << "weight=" << weight << ","
        << "position=(" << pos.getRow() << "," << pos.getCol() << ")" << "]";
    return oss.str();
}

//!----------------------------------------------
//! Lớp Vehicle
//!----------------------------------------------
Vehicle::Vehicle(int quantity, int weight, Position pos,
                 VehicleType vehicleType)
    : Unit(quantity, weight, pos), vehicleType(vehicleType) {
    attackScore = 0;
}

int Vehicle::getAttackScore() {
    double score = (vehicleType * 304.0 + quantity * weight) / 30.0;
    return CommonFunction::getCorrectRounded(score);
}

string Vehicle::str() const {
    ostringstream oss;
    oss << "Vehicle[" << "vehicleType=" << vehicleTypeToString(vehicleType)
        << ","
        << "quantity=" << quantity << ","
        << "weight=" << weight << ","
        << "position=(" << pos.getRow() << "," << pos.getCol() << ")" << "]";
    return oss.str();
}

//!----------------------------------------------
//! Lớp UnitList
//!----------------------------------------------
UnitList::UnitList(int capacity) : countInfantry(0), countVehicle(0), head(nullptr), capacity(capacity) {}

UnitList::UnitList(const UnitList &other) {
    head = nullptr;
    tail = nullptr;
    capacity = other.capacity;
    countInfantry = other.countInfantry;
    countVehicle = other.countVehicle;

    Node *current = other.head;
    while (current) {
        this->insert(current->unit);
        current = current->next;
    }
}

bool UnitList::insert(Unit *unit) {
    if (!unit)
        return false;

    Node *current = head;

    if (!unit->getType()) {
        Infantry *tmp1 = static_cast<Infantry *>(unit);
        while (current) {
            if (!current->unit->getType()) {
                Infantry *tmp3 = static_cast<Infantry *>(current->unit);
                if (tmp3->getInfantryType() == tmp1->getInfantryType()) {
                    if (tmp3->getWeight() < tmp1->getWeight())
                        current->unit->setWeight(unit->getWeight());
                    current->unit->setQuantity(current->unit->getQuantity() +
                                               unit->getQuantity());
                    current->unit->setAttackScore(
                        current->unit->getAttackScore());
                    return false;
                }
            }

            current = current->next;
        }

        if (capacity <= 0) {
            return false;
        }

        Node *nextNode = new Node(unit);
        if (!head) {
            head = nextNode;
            tail = nextNode;
            countInfantry++;
            capacity--;
            return true;
        }
        nextNode->next = head;
        head = nextNode;
        countInfantry++;
        capacity--;
        return true;
    }

    else if (unit->getType()) {
        Vehicle *tmp2 = static_cast<Vehicle *>(unit);
        while (current) {
            if (current->unit->getType()) {
                Vehicle *tmp4 = static_cast<Vehicle *>(current->unit);
                if (tmp4->getVehicleType() == tmp2->getVehicleType()) {
                    if (tmp4->getWeight() < tmp2->getWeight())
                        current->unit->setWeight(unit->getWeight());
                    current->unit->setQuantity(current->unit->getQuantity() +
                                               unit->getQuantity());
                    current->unit->setAttackScore(
                        current->unit->getAttackScore());
                    return false;
                }
            }

            current = current->next;
        }

        if (capacity <= 0) {
            return false;
        }

        Node *nextNode = new Node(unit);
        if (!head) {
            head = nextNode;
            tail = nextNode;
            countVehicle++;
            capacity--;
            return true;
        }

        tail->next = nextNode;
        tail = nextNode;
        countVehicle++;
        capacity--;
        return true;
    }

    else {
        return false;
    }
}

bool UnitList::isContain(VehicleType vehicleType) {
    Node *current = head;
    while (current) {
        if (!current->unit->getType()) {
            current = current->next;
            continue;
        }
        Vehicle *tmp = static_cast<Vehicle *>(current->unit);

        if (tmp->getVehicleType() == vehicleType) {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool UnitList::isContain(InfantryType infantryType) {
    Node *current = head;
    Infantry *tmp;

    while (current) {
        if (!current->unit->getType()) {
            tmp = static_cast<Infantry *>(current->unit);
            if (tmp->getInfantryType() == infantryType) {
                return true;
            }
        }

        current = current->next;
    }

    return false;
}

string UnitList::str() const {
    ostringstream oss;
    Node *current = head;

    oss << "UnitList[" << "count_vehicle=" << countVehicle
        << ";count_infantry=" << countInfantry;
    if (current) {
        oss << ";";
    }
    while (current) {
        oss << current->unit->str();
        if (current->next)
            oss << ",";
        current = current->next;
    }
    oss << "]";
    return oss.str();
}

void UnitList::remove(Unit *unit) {
    Node *current = head;
    Node *prev = nullptr;

    while (current) {
        bool match = false;
        if (!current->unit->getType() && !unit->getType()) {
            Infantry *infantryUnit = static_cast<Infantry *>(unit);
            Infantry *currentInfantry = static_cast<Infantry *>(current->unit);
            if (currentInfantry && currentInfantry->getInfantryType() ==
                                       infantryUnit->getInfantryType()) {
                match = true;
            }
        }

        else if (current->unit->getType() && unit->getType()) {
            Vehicle *vehicleUnit = static_cast<Vehicle *>(unit);
            Vehicle *currentVehicle = static_cast<Vehicle *>(current->unit);
            if (currentVehicle && currentVehicle->getVehicleType() ==
                                      vehicleUnit->getVehicleType()) {
                match = true;
            }
        }

        if (match) {
            Node *toDelete = current;
            if (!prev)
                head = current->next;
            else
                prev->next = current->next;
            if (current == tail)
                tail = prev;

            if (!unit->getType())
                countInfantry--;
            else
                countVehicle--;
            current = current->next;
            delete toDelete;
            capacity++;
            return;
        } else {
            prev = current;
            current = current->next;
        }
    }
}

UnitList::~UnitList() {
    Node *current = head;

    while (current) {
        Node *nextNode = current->next;
        delete current;
        current = nextNode;
    }

    head = nullptr;
    tail = nullptr;
}

//!----------------------------------------------
//! Lớp Army
//!----------------------------------------------
Army::Army(Unit **unitArray, int size, string name, BattleField *battleField)
    : name(name), battleField(battleField) {
    LF = 0, EXP = 0;
    if (unitArray == nullptr || size <= 0) {
        unitList = new UnitList(0);
        return;
    }

    for (int i = 0; i < size; i++) {
        Unit *unit = unitArray[i];

        if (unit->getType()) {
            unit->setAttackScore(unit->getAttackScore());
            LF += unit->getAttackScoret();
        } else {
            unit->setAttackScore(unit->getAttackScore());
            EXP += unit->getAttackScoret();
        }
    }

    LF = max(0, min(LF, 1000));
    EXP = max(0, min(EXP, 500));

    int S = LF + EXP;
    if (checkSpecialNumber(S))
        S = 12;
    else
        S = 8;

    Army::unitList = new UnitList(S);

    for (int i = 0; i < size; i++) {
        if (!unitArray[i])
            continue;

        unitList->insert(unitArray[i]);
    }
}

bool Army::update = false;

Army::~Army() { delete unitList; }

string Army::str() const {
    return "Army[name=" + name + ",LF=" + to_string(LF) +
           ",EXP=" + to_string(EXP) + "]";
}

void Army::check() {
    LF = max(0, min(LF, 1000));
    EXP = max(0, min(EXP, 500));
}

void Army::reset() {
    EXP = 0, LF = 0;
    Node *current = unitList->getHead();

    while (current) {
        if (!current->unit->getType()) {
            current->unit->setAttackScore(current->unit->getAttackScore());
            EXP += current->unit->getAttackScoret();
        } else {
            current->unit->setAttackScore(current->unit->getAttackScore());
            LF += current->unit->getAttackScoret();
        }
        current = current->next;
    }
}

//!----------------------------------------------
//! CLASS LiberationArmy
//!----------------------------------------------
LiberationArmy::LiberationArmy(Unit **unitArray, int size, string name,
                               BattleField *battleField)
    : Army(unitArray, size, name, battleField) {}

void LiberationArmy::fight(Army *enemy, bool defense) {
    if (!defense) {
        vector<Unit *> units1;
        vector<Unit *> units2;
        Node *current = unitList->getHead();

        EXP = CommonFunction::getCorrectRounded(EXP * 1.5);
        LF = CommonFunction::getCorrectRounded(LF * 1.5);
        EXP = max(0, min(EXP, 500));
        LF = max(0, min(LF, 1000));

        while (current) {
            if (current->unit->getType()) {
                units2.push_back(current->unit);
            } else
                units1.push_back(current->unit);
            current = current->next;
        }

        units1 = knapsack(units1, enemy->getEXP());
        units2 = knapsack(units2, enemy->getLF());

        bool check1 = !units1.empty();
        bool check2 = !units2.empty();

        if (check1 && check2) {
            for (Unit *unit : units1)
                unitList->remove(unit);
            for (Unit *unit : units2)
                unitList->remove(unit);

            update = true;
        }

        else if ((check1 || check2)) {
            current = unitList->getHead();
            if (!check1 && EXP > enemy->getEXP()) {
                while (current) {
                    if (!current->unit->getType()) {
                        Unit *tmp = current->unit;
                        current = current->next;
                        unitList->remove(tmp);
                        continue;
                    }
                    current = current->next;
                }

                for (Unit *unit : units2)
                    unitList->remove(unit);

                update = true;
            }

            else if (!check2 && LF > enemy->getLF()) {
                while (current) {
                    if (current->unit->getType()) {
                        Unit *tmp = current->unit;
                        current = current->next;
                        unitList->remove(tmp);
                        continue;
                    }
                    current = current->next;
                }

                for (Unit *unit : units1)
                    unitList->remove(unit);

                update = true;
            }
        }

        updateScoreforAttack(enemy);
    }

    else {
        EXP = CommonFunction::getCorrectRounded(EXP * 1.3);
        LF = CommonFunction::getCorrectRounded(LF * 1.3);
        EXP = max(0, min(EXP, 500));
        LF = max(0, min(LF, 1000));
        updateScoreforDefense(enemy);
    }
}

string LiberationArmy::str() const {
    ostringstream oss;

    oss << "LiberationArmy["
        << "LF=" << LF << ","
        << "EXP=" << EXP << ","
        << "unitList=" << unitList->str();
    if (battleField == nullptr)
        oss << ",battleField=]";
    else
        oss << ",battleField=" << battleField->str() << "]";
    return oss.str();
}

int LiberationArmy::nearestFibonacci(int value) {
    if (value == 0) {
        return 0;
    }

    int first = 0;
    int second = 1;
    int third = 1;

    while (third <= value) {
        first = second;
        second = third;
        third = first + second;
    }

    return third;
}

vector<Unit *> LiberationArmy::knapsack(vector<Unit *> units, int minScore) {
    int sum_max = 0;
    vector<Unit *> result;
    if (minScore == 0) {
        Unit *minUnit = nullptr;

        for (Unit *unit : units) {
            if (!minUnit && unit->getAttackScoret() > 0) {
                minUnit = unit;
            } else if (minUnit &&
                       unit->getAttackScoret() < minUnit->getAttackScoret() &&
                       unit->getAttackScoret() > 0) {
                minUnit = unit;
            }
        }

        if (minUnit) {
            result.push_back(minUnit);
        }
        return result;
    }

    for (Unit *unit : units) {
        sum_max += unit->getAttackScoret();
    }
    vector<int> sum(sum_max + 1, INT_MAX);
    vector<int> trace(sum_max + 1, 0);
    sum[0] = 0;

    for (Unit *unit : units) {
        int num = unit->getAttackScoret();
        for (int i = sum_max; i >= num; i--) {
            if (sum[i - num] != INT_MAX && sum[i] > sum[i - num] + num) {
                sum[i] = sum[i - num] + num;
                trace[i] = num;
            }
        }
    }

    for (int i = minScore + 1; i <= sum_max; i++) {
        if (sum[i] != INT_MAX) {
            int current = i;
            while (current > 0) {
                for (Unit *unit : units) {
                    if (unit->getAttackScoret() == trace[current]) {
                        result.push_back(unit);
                        break;
                    }
                }
                current -= trace[current];
            }
            return result;
        }
    }
    return result;
}

void LiberationArmy::updateScoreforAttack(Army *enemy) {
    Node *current = nullptr;

    if (update) {
        vector<Unit *> tmpStack;
        Node *currentEnemy = enemy->getUnitList()->getHead();

        while (currentEnemy) {
            tmpStack.push_back(currentEnemy->unit);
            currentEnemy = currentEnemy->next;
        }

        for (auto it = tmpStack.rbegin(); it != tmpStack.rend(); ++it) {
            if ((*it)->getType()) {
                VehicleType tmp1 =
                    static_cast<Vehicle *>(*it)->getVehicleType();
                if (unitList->insert(*it) || unitList->isContain(tmp1))
                    enemy->getUnitList()->remove(*it);
            }

            else {
                InfantryType tmp2 =
                    static_cast<Infantry *>(*it)->getInfantryType();
                if (unitList->insert(*it) || unitList->isContain(tmp2))
                    enemy->getUnitList()->remove(*it);
            }
        }
    }

    else {
        current = unitList->getHead();
        while (current) {
            int modWeight = CommonFunction::getCorrectRounded(
                current->unit->getWeight() * 0.9);
            current->unit->setWeight(modWeight);
            current = current->next;
        }
    }

    reset();
    EXP = max(0, min(EXP, 500));
    LF = max(0, min(LF, 1000));
    if (update)
        enemy->fight(this, true);
}

void LiberationArmy::updateScoreforDefense(Army *enemy) {
    Node *current = unitList->getHead();
    bool check = false;

    bool expWin = (EXP >= enemy->getEXP());
    bool lfWin = (LF >= enemy->getLF());

    while (!expWin && !lfWin) {
        current = unitList->getHead();
        while (current) {
            int fibQuantity = nearestFibonacci(current->unit->getQuantity());
            current->unit->setQuantity(fibQuantity);
            current = current->next;
        }

        reset();
        EXP = max(0, min(EXP, 500));
        LF = max(0, min(LF, 1000));

        expWin = (EXP >= enemy->getEXP());
        lfWin = (LF >= enemy->getLF());
    }

    if (!expWin || !lfWin) {
        current = unitList->getHead();
        while (current) {
            int modQuantity = CommonFunction::getCorrectRounded(
                current->unit->getQuantity() * 0.9);
            current->unit->setQuantity(modQuantity);
            current = current->next;
        }
    } else
        return;

    reset();
    EXP = max(0, min(EXP, 500));
    LF = max(0, min(LF, 1000));
}

//!----------------------------------------------
//! CLASS ARVN
//!----------------------------------------------
ARVN::ARVN(Unit **unitArray, int size, string name, BattleField *battleField)
    : Army(unitArray, size, name, battleField) {}

void ARVN::fight(Army *enemy, bool defense) {
    if (!defense) {
        updateScoreforAttack(enemy);
    } else {
        updateScoreforDefense(enemy);
    }
}

string ARVN::str() const {
    ostringstream oss;

    oss << "ARVN["
        << "LF=" << LF << ","
        << "EXP=" << EXP << ","
        << "unitList=" << unitList->str();
    if (battleField == nullptr)
        oss << ",battleField=]";
    else
        oss << ",battleField=" << battleField->str() << "]";
    return oss.str();
}

void ARVN::updateScoreforAttack(Army *enemy) {
    Node *current = unitList->getHead();
    Node *tmp = nullptr;

    while (current) {
        double modQuantity = max(1, CommonFunction::getCorrectRounded(
                                        current->unit->getQuantity() * 0.8));
        bool erase = false;
        if (modQuantity == 1) {
            tmp = current->next;
            unitList->remove(current->unit);
            erase = true;
        } else
            current->unit->setQuantity(modQuantity);

        if (erase) {
            current = tmp;
            continue;
        }

        current = current->next;
    }

    reset();
    EXP = max(0, min(EXP, 500));
    LF = max(0, min(LF, 1000));
}

void ARVN::updateScoreforDefense(Army *enemy) {
    if (update) {
        Node *current = unitList->getHead();
        while (current) {
            double modWeight = max(0, CommonFunction::getCorrectRounded(
                                          current->unit->getWeight() * 0.8));
            current = current->next;
        }
        reset();
        EXP = max(0, min(EXP, 500));
        LF = max(0, min(LF, 1000));
    }
}

//!-----------------------------------------------------
//! Lớp TerrainElement và các lớp con
//!-----------------------------------------------------
TerrainElement::TerrainElement(Position pos) : pos(pos) {}

TerrainElement::~TerrainElement() {}

// Road: không có hiệu ứng
void Road::getEffect(Army *army) { return; }

void Mountain::getEffect(Army *army) {
    if (army == nullptr)
        return;

    Node *current = army->getUnitList()->getHead();
    if (dynamic_cast<LiberationArmy *>(army)) {
        while (current) {
            double distance =
                calculateDistance(current->unit->getCurrentPosition(), pos);

            if (distance <= 2) {
                double modScore =
                    (current->unit->getType())
                        ? (current->unit->getAttackScoret() * 0.1)
                        : (current->unit->getAttackScoret() * 0.3);
                if (current->unit->getType())
                    army->setLF(army->getLF() - CommonFunction::getCorrectRounded(modScore));
                else
                    army->setEXP(army->getEXP() + CommonFunction::getCorrectRounded(modScore));
            }

            current = current->next;
        }
    }

    else {
        while (current) {
            double distance =
                calculateDistance(current->unit->getCurrentPosition(), pos);

            if (distance <= 4) {
                double modScore =
                    (current->unit->getType())
                        ? (current->unit->getAttackScoret() * 0.05)
                        : (current->unit->getAttackScoret() * 0.2);
                if (current->unit->getType())
                    army->setLF(army->getLF() - CommonFunction::getCorrectRounded(modScore));
                else
                    army->setEXP(army->getEXP() + CommonFunction::getCorrectRounded(modScore));
            }

            current = current->next;
        }
    }

    army->check();
}

void River::getEffect(Army *army) {
    if (army == nullptr)
        return;
    Node *current = army->getUnitList()->getHead();

    while (current) {
        if (!current->unit->getType()) {
            double distance =
                calculateDistance(current->unit->getCurrentPosition(), pos);

            if (distance <= 2) {
                int modScore = CommonFunction::getCorrectRounded(
                    current->unit->getAttackScoret() -
                    current->unit->getAttackScoret() * 0.1);
                current->unit->setAttackScore(modScore);
            }
        }
        current = current->next;
    }
}

void Urban::getEffect(Army *army) {
    if (army == nullptr)
        return;
    Node *current = army->getUnitList()->getHead();

    if (dynamic_cast<LiberationArmy *>(army)) {
        while (current) {
            double distance =
                calculateDistance(current->unit->getCurrentPosition(), pos);

            if (distance <= 5 && !current->unit->getType()) {
                Infantry *infantry = dynamic_cast<Infantry *>(current->unit);
                if (infantry->getInfantryType() == SPECIALFORCES ||
                    infantry->getInfantryType() == REGULARINFANTRY) {
                    double modScore =
                        (distance) ? (current->unit->getAttackScoret() * 2.0) /
                                         distance
                                   : 0;
                    current->unit->setAttackScore(
                        CommonFunction::getCorrectRounded(
                            current->unit->getAttackScoret() + modScore));
                }
            }

            else if (distance <= 2 && current->unit->getType()) {
                Vehicle *vehicle = dynamic_cast<Vehicle *>(current->unit);
                if (vehicle->getVehicleType() == ARTILLERY) {
                    double modScore = current->unit->getAttackScoret() * 0.5;
                    current->unit->setAttackScore(
                        CommonFunction::getCorrectRounded(
                            current->unit->getAttackScoret() - modScore));
                }
            }

            current = current->next;
        }
    }

    else {
        while (current) {
            if (!current->unit->getType()) {
                Infantry *infantry = static_cast<Infantry *>(current->unit);
                double distance =
                    calculateDistance(current->unit->getCurrentPosition(), pos);

                if (distance <= 3 &&
                    infantry->getInfantryType() == REGULARINFANTRY) {
                    double modScore =
                        (distance) ? (current->unit->getAttackScoret() * 3.0) /
                                         (2.0 * distance)
                                   : 0;
                    current->unit->setAttackScore(
                        CommonFunction::getCorrectRounded(
                            current->unit->getAttackScoret() + modScore));
                }
            }

            current = current->next;
        }
    }
}

void Fortification::getEffect(Army *army) {
    if (army == nullptr)
        return;
    Node *current = army->getUnitList()->getHead();

    if (dynamic_cast<LiberationArmy *>(army)) {
        while (current) {
            double distance =
                calculateDistance(current->unit->getCurrentPosition(), pos);

            if (distance <= 2) {
                int modScore = CommonFunction::getCorrectRounded(
                    current->unit->getAttackScoret() -
                    current->unit->getAttackScoret() * 0.2);
                current->unit->setAttackScore(modScore);
            }

            current = current->next;
        }
    }

    else {
        while (current) {
            double distance =
                calculateDistance(current->unit->getCurrentPosition(), pos);

            if (distance <= 2) {
                int modScore = CommonFunction::getCorrectRounded(
                    current->unit->getAttackScoret() +
                    current->unit->getAttackScoret() * 0.2);
                current->unit->setAttackScore(modScore);
            }

            current = current->next;
        }
    }
}

void SpecialZone::getEffect(Army *army) {
    if (army == nullptr)
        return;
    Node *current = army->getUnitList()->getHead();

    while (current) {
        double distance =
            calculateDistance(current->unit->getCurrentPosition(), pos);

        if (distance <= 1) {
            current->unit->setAttackScore(0);
        }

        current = current->next;
    }
}

//!----------------------------------------------
//! CLASS BattleField
//!----------------------------------------------
BattleField::BattleField(int n_rows, int n_cols,
                         const vector<Position *> &arrayForest,
                         const vector<Position *> &arrayRiver,
                         const vector<Position *> &arrayFortification,
                         const vector<Position *> &arrayUrban,
                         const vector<Position *> &arraySpecialZone)
    : n_rows(n_rows), n_cols(n_cols) {
    terrain = new TerrainElement **[n_rows];
    for (int i = 0; i < n_rows; ++i) {
        terrain[i] = new TerrainElement *[n_cols];
        for (int j = 0; j < n_cols; ++j) {
            terrain[i][j] = nullptr;
        }
    }

    for (Position *pos : arrayForest)
        if (checkValidPos(pos))
            terrain[pos->getRow()][pos->getCol()] = new Mountain(*pos);
    for (Position *pos : arrayRiver)
        if (checkValidPos(pos))
            terrain[pos->getRow()][pos->getCol()] = new River(*pos);
    for (Position *pos : arrayFortification)
        if (checkValidPos(pos))
            terrain[pos->getRow()][pos->getCol()] = new Fortification(*pos);
    for (Position *pos : arrayUrban)
        if (checkValidPos(pos))
            terrain[pos->getRow()][pos->getCol()] = new Urban(*pos);
    for (Position *pos : arraySpecialZone)
        if (checkValidPos(pos))
            terrain[pos->getRow()][pos->getCol()] = new SpecialZone(*pos);

    for (int i = 0; i < n_rows; ++i) {
        for (int j = 0; j < n_cols; ++j) {
            if (terrain[i][j] == nullptr)
                terrain[i][j] = new Road(Position(i, j));
        }
    }
}

BattleField::~BattleField() {
    for (int i = 0; i < n_rows; ++i) {
        for (int j = 0; j < n_cols; ++j) {
            delete terrain[i][j];
        }
        delete[] terrain[i];
    }
    delete[] terrain;
}

TerrainElement *BattleField::getElement(int r, int c) const {
    if (r < 0 || r >= n_rows || c < 0 || c >= n_cols) {
        return nullptr;
    }
    return terrain[r][c];
}

bool BattleField::checkValidPos(Position *pos) const {
    if (pos->getRow() >= 0 && pos->getRow() < n_rows && pos->getCol() >= 0 &&
        pos->getCol() < n_cols) {
        if (terrain[pos->getRow()][pos->getCol()] != nullptr) {
            return false;
        }

        return true;
    }

    return false;
}

string BattleField::str() const {
    ostringstream oss;
    oss << "BattleField[n_rows=" << n_rows << ",n_cols=" << n_cols << "]";

    return oss.str();
}

//!-----------------------------------------------------
//! CLASS Configuration
//!-----------------------------------------------------
Configuration::Configuration(const std::string &filepath) {
    ifstream inputFile(filepath);
    string line;

    if (!inputFile.is_open()) {
        cout << "Error opening file: " << filepath << endl;
        return;
    }

    while (getline(inputFile, line)) {
        replaceChars(line);
        if (line.empty())
            continue;
        stringstream ss(line);
        string temp;
        int r = 0, c = 0;

        while (ss >> temp) {
            if (temp == "NUM_ROWS") {
                ss >> num_rows;
            } else if (temp == "NUM_COLS") {
                ss >> num_cols;
            } else if (temp == "EVENT_CODE") {
                ss >> eventCode;
                eventCode = max(0, eventCode % 100);
            } else if (temp == "ARRAY_FOREST") {
                while (ss >> r >> c)
                    arrayForest.push_back(new Position(r, c));
            } else if (temp == "ARRAY_RIVER") {
                while (ss >> r >> c)
                    arrayRiver.push_back(new Position(r, c));
            } else if (temp == "ARRAY_FORTIFICATION") {
                while (ss >> r >> c)
                    arrayFortification.push_back(new Position(r, c));
            } else if (temp == "ARRAY_URBAN") {
                while (ss >> r >> c)
                    arrayUrban.push_back(new Position(r, c));
            } else if (temp == "ARRAY_SPECIAL_ZONE") {
                while (ss >> r >> c)
                    arraySpecialZone.push_back(new Position(r, c));
            } else if (temp == "UNIT_LIST") {
                string unitName;
                int quantity, weight, x, y, armyBelong;
                int i = 0, j = 0;

                while (ss >> unitName >> quantity >> weight >> x >> y >>
                       armyBelong) {
                    Unit *unit = nullptr;
                    Position pos(x, y);

                    try {
                        InfantryType itype =
                            Infantry::stringToInfantryType(unitName);
                        unit = new Infantry(quantity, weight, pos, itype);
                    } catch (const invalid_argument &) {
                        try {
                            VehicleType vtype =
                                Vehicle::stringToVehicleType(unitName);
                            unit = new Vehicle(quantity, weight, pos, vtype);
                        } catch (const invalid_argument &) {
                            cout << "Invalid unit type: " << unitName << endl;
                            continue;
                        }
                    }

                    if (armyBelong)
                        ARVNUnits[i++] = unit;
                    else
                        liberationUnits[j++] = unit;
                }

                ARVNUnits[i] = nullptr;
                numARVNUnits = i;
                liberationUnits[j] = nullptr;
                numLiberationUnits = j;
            } else
                cout << "Unknown command: " << temp << endl;
        }
    }

    if (num_rows < 0 || num_cols < 0) {
        cout << "Invalid battlefield dimensions: " << num_rows << "x"
             << num_cols << endl;
    }

    inputFile.close();
}

Configuration::~Configuration() {
    for (int i = 0; i < arrayForest.size(); ++i)
        delete arrayForest[i];
    for (int i = 0; i < arrayRiver.size(); ++i)
        delete arrayRiver[i];
    for (int i = 0; i < arrayFortification.size(); ++i)
        delete arrayFortification[i];
    for (int i = 0; i < arrayUrban.size(); ++i)
        delete arrayUrban[i];
    for (int i = 0; i < arraySpecialZone.size(); ++i)
        delete arraySpecialZone[i];
    for (int i = 0; i < 100; ++i) {
        if (liberationUnits[i] == nullptr)
            break;
        delete liberationUnits[i];
    }  
    for (int i = 0; i < 100; ++i) {
        if (ARVNUnits[i] == nullptr)
            break;
        delete ARVNUnits[i];
    }
}

string Configuration::str() const {
    ostringstream oss;
    oss << "["
        << "num_rows=" << num_rows << ","
        << "num_cols=" << num_cols << ","
        << "arrayForest=[" << createOutput(arrayForest) << "],arrayRiver=["
        << createOutput(arrayRiver) << "],arrayFortification=["
        << createOutput(arrayFortification) << "],arrayUrban=["
        << createOutput(arrayUrban) << "],arraySpecialZone=["
        << createOutput(arraySpecialZone) << "],liberationUnits=["
        << createOutput(liberationUnits) << "],ARVNUnits=["
        << createOutput(ARVNUnits) << "],eventCode=" << eventCode << "]";

    return oss.str();
}

//!-----------------------------------------------------
//! Lớp HCMCampaign
//!-----------------------------------------------------
HCMCampaign::HCMCampaign(const string &config_file_path) {
    config = new Configuration(config_file_path);
    battleField = new BattleField(
        config->getNumRows(), config->getNumCols(),
        config->getForestPositions(), config->getRiverPositions(),
        config->getFortificationPositions(), config->getUrbanPositions(),
        config->getSpecialZonePositions());

    Unit **liberationUnits = createData(config->getLiberationUnits(), 
                                        config->getNumLiberationUnits());
    Unit **arvnUnits = createData(config->getARVNUnits(), 
                                   config->getNumARVNUnits());

    liberationArmy = new LiberationArmy(liberationUnits, config->getNumLiberationUnits(),
                           "LiberationArmy", battleField);
    ARVNarmy = new ARVN(arvnUnits, config->getNumARVNUnits(), "ARVNArmy",
                        battleField);

    delete[] liberationUnits;
    delete[] arvnUnits;
}

void HCMCampaign::run() {
    for (int i = 0; i < config->getNumRows(); ++i) {
        for (int j = 0; j < config->getNumCols(); ++j) {
            battleField->getElement(i, j)->getEffect(liberationArmy);
            battleField->getElement(i, j)->getEffect(ARVNarmy);
        }
    }

    if (config->getEventCode() < 75) {
        liberationArmy->fight(ARVNarmy, false);
    } else {
        ARVNarmy->fight(liberationArmy, false);
        liberationArmy->fight(ARVNarmy, true);
        liberationArmy->fight(ARVNarmy, false);
    }

    UpdateScore(liberationArmy);
    UpdateScore(ARVNarmy);
}

void HCMCampaign::UpdateScore(Army *army) {
    Node *cur = army->getUnitList()->getHead();
    while (cur) {
        if (cur->unit->getAttackScoret() <= 5) {
            Unit *tmp = cur->unit;

            cur = cur->next;
            army->getUnitList()->remove(tmp);
        }

        else
            cur = cur->next;
    }

    army->reset();
    army->check();
}

string HCMCampaign::printResult() {
    ostringstream oss;
    oss << "LIBERATIONARMY[LF=" << liberationArmy->getLF()
        << ",EXP=" << liberationArmy->getEXP() << "]-"
        << "ARVN[LF=" << ARVNarmy->getLF() << ",EXP=" << ARVNarmy->getEXP()
        << "]";

    return oss.str();
}
