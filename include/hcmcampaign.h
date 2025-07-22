// The library here is concretely set, students are not allowed to include any
// other libraries.
#ifndef _H_HCM_CAMPAIGN_H_
#define _H_HCM_CAMPAIGN_H_

#include "main.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// Forward declaration
class Unit;
class UnitList;
class Army;
class TerrainElement;

class Vehicle;
class Infantry;

class LiberationArmy;
class ARVN;

class Position;

class Road;
class Mountain;
class River;
class Urban;
class Fortification;
class SpecialZone;

class BattleField;

class HCMCampaign;
class Configuration;

enum VehicleType {
    TRUCK,
    MORTAR,
    ANTIAIRCRAFT,
    ARMOREDCAR,
    APC,
    ARTILLERY,
    TANK
};
enum InfantryType {
    SNIPER,
    ANTIAIRCRAFTSQUAD,
    MORTARSQUAD,
    ENGINEER,
    SPECIALFORCES,
    REGULARINFANTRY
};

class CommonFunction {
  public:
    static int getCorrectRounded(double score) {
        if (score - round(score) >= 1e-7) {
            return ceil(score);
        }
        return round(score);
    }
};

//!-----------------------------------------------------
//! CLASS Position
//!-----------------------------------------------------
class Position {
  private:
    int r, c;

  public:
    Position(int r = 0, int c = 0);
    Position(const string &str_pos);
    int getRow() const;
    int getCol() const;
    void setRow(int r);
    void setCol(int c);
    string str() const;
};

//!-----------------------------------------------------
//! CLASS Unit
//!-----------------------------------------------------
class Unit {
  protected:
    int quantity, weight;
    Position pos;
    int attackScore;
    enum UnitType { INFANTRY, VEHICLE };

  public:
    Unit(int quantity, int weight, Position pos);
    virtual ~Unit();
    virtual int getAttackScore() = 0;
    Position getCurrentPosition() const;
    virtual string str() const = 0;
    virtual UnitType getType() const = 0;

    // Getter and setter
    int getQuantity() { return quantity; }
    void setQuantity(int quantity) { this->quantity = quantity; }
    int getWeight() { return weight; }
    void setWeight(int weight) { this->weight = weight; }
    void setAttackScore(int attackScore) { this->attackScore = attackScore; }
    int getAttackScoret() { return attackScore; }
};

//!-----------------------------------------------------
//! CLASS Infantry : public Unit
//!-----------------------------------------------------
class Infantry : public Unit {
  private:
    bool isPerfectSquare(int num) const {
        int root = sqrt(num);
        return root * root == num;
    }

    int updateScore(int score) {
        if (score >= 0 && score < 10)
            return score;
        int sum = 0;
        while (score > 0) {
            sum += score % 10;
            score /= 10;
        }
        return updateScore(sum);
    }

    int caculateScore(int infantryType, int quantity, int weight) {
        double score = (infantryType * 56.0 + quantity * weight);
        score = CommonFunction::getCorrectRounded(score);
        return score;
    }

  protected:
    InfantryType infantryType;

  public:
    // * Constructor: khởi tạo với quantity, weight, pos và infantryType.
    Infantry(int quantity, int weight, Position pos, InfantryType infantryType);

    int getAttackScore() override;
    string str() const override;
    UnitType getType() const override { return INFANTRY; }

    InfantryType getInfantryType() const { return infantryType; }
    int getQuantity() const { return quantity; }
    int getWeight() const { return weight; }
    Position getPosition() const { return pos; }

    void setQuantity(int value) { quantity = value; }
    void setWeight(int value) { weight = value; }
    void setPosition(Position value) { pos = value; }

    static string infantryTypeToString(InfantryType it) {
        switch (it) {
        case SNIPER:
            return "SNIPER";
        case ANTIAIRCRAFTSQUAD:
            return "ANTIAIRCRAFTSQUAD";
        case MORTARSQUAD:
            return "MORTARSQUAD";
        case ENGINEER:
            return "ENGINEER";
        case SPECIALFORCES:
            return "SPECIALFORCES";
        case REGULARINFANTRY:
            return "REGULARINFANTRY";
        default:
            return "UNKNOWN";
        }
    }

    static InfantryType stringToInfantryType(const string &s) {
        if (s == "SNIPER")
            return SNIPER;
        if (s == "ANTIAIRCRAFTSQUAD")
            return ANTIAIRCRAFTSQUAD;
        if (s == "MORTARSQUAD")
            return MORTARSQUAD;
        if (s == "ENGINEER")
            return ENGINEER;
        if (s == "SPECIALFORCES")
            return SPECIALFORCES;
        if (s == "REGULARINFANTRY")
            return REGULARINFANTRY;
        throw invalid_argument("Invalid infantry type: " + s);
    }
};

//!-----------------------------------------------------
//! CLASS Vehicle : public Unit
//!-----------------------------------------------------
class Vehicle : public Unit {
  private:
    // Unit* data;
  protected:
    VehicleType vehicleType;

  public:
    Vehicle(int quantity, int weight, Position pos, VehicleType vehicleType);
    // * Constructor: khởi tạo với quantity, weight, pos và infantryType

    int getAttackScore() override;

    string str() const override;
    VehicleType getVehicleType() const { return vehicleType; }
    void setVehicleType(VehicleType value) { vehicleType = value; }
    UnitType getType() const override { return VEHICLE; }

    static string vehicleTypeToString(VehicleType vt) {
        switch (vt) {
        case TRUCK:
            return "TRUCK";
        case MORTAR:
            return "MORTAR";
        case ANTIAIRCRAFT:
            return "ANTIAIRCRAFT";
        case ARMOREDCAR:
            return "ARMOREDCAR";
        case APC:
            return "APC";
        case ARTILLERY:
            return "ARTILLERY";
        case TANK:
            return "TANK";
        default:
            return "UNKNOWN";
        }
    }

    static VehicleType stringToVehicleType(const string &s) {
        if (s == "TRUCK")
            return TRUCK;
        if (s == "MORTAR")
            return MORTAR;
        if (s == "ANTIAIRCRAFT")
            return ANTIAIRCRAFT;
        if (s == "ARMOREDCAR")
            return ARMOREDCAR;
        if (s == "APC")
            return APC;
        if (s == "ARTILLERY")
            return ARTILLERY;
        if (s == "TANK")
            return TANK;
        throw invalid_argument("Invalid vehicle type: " + s);
    }
};

//!-----------------------------------------------------
//! CLASS UnitList
//!-----------------------------------------------------
class Node {
  public:
    Unit *unit;
    Node *next;
    Node(Unit *data) : unit(data), next(nullptr) {}
};

class UnitList {
  private:
    int capacity;
    Node *head;
    Node *tail;
    int countVehicle, countInfantry;

  public:
    UnitList(int capacity);
    UnitList(const UnitList &other);
    ~UnitList();
    bool insert(Unit *unit);
    bool isContain(VehicleType vehicleType);
    bool isContain(InfantryType infantryType);

    // Getter and setter
    Node *getHead() { return head; }
    Node *getTail() { return tail; }
    string str() const;
    int getCapacity() const { return capacity; }
    void setCapacity(int capacity) { this->capacity = capacity; }
    void remove(Unit *unit);
};

//!-----------------------------------------------------
//! CLASS Army
//!-----------------------------------------------------
class Army {
  private:
    bool checkSpecialNumber(int S) {
        int count = 0;
        while (count < 3) {
            int tmp = S;
            switch (count) {
            case 0:
                while (tmp / 3) {
                    if (tmp % 3 != 1 && tmp % 3 != 0)
                        break;
                    tmp /= 3;
                }
                if (tmp == 0 || tmp == 1)
                    return true;
                break;
            case 1:
                while (tmp / 5) {
                    if (tmp % 5 != 1 && tmp % 5 != 0)
                        break;
                    tmp /= 5;
                }
                if (tmp == 0 || tmp == 1)
                    return true;
                break;
            case 2:
                while (tmp / 7) {
                    if (tmp % 7 != 1 && tmp % 7 != 0)
                        break;
                    tmp /= 7;
                }
                if (tmp == 0 || tmp == 1)
                    return true;
                break;
            }
            count++;
        }
        return false;
    }

  protected:
    int LF, EXP;
    string name;
    BattleField *battleField;
    enum ArmyType { LIBERATION, ARVNN };
    UnitList *unitList;

  public:
    static bool update;
    Army(Unit **unitArray, int size, string name, BattleField *battleField);
    virtual ~Army();
    virtual void fight(Army *enemy, bool defense = false) = 0;
    virtual string str() const = 0;

    void reset();
    void resetAfterFight();
    void check();

    // Getter and setter
    int getLF() { return LF; }
    void setLF(int lf) { LF = lf; }
    int getEXP() { return EXP; }
    void setEXP(int exp) { EXP = exp; }
    UnitList *getUnitList() const { return unitList; }
};

//!-----------------------------------------------------
//! CLASS LiberationArmy
//!-----------------------------------------------------
class LiberationArmy : public Army {
  private:
    int nearestFibonacci(int value);
    vector<Unit *> knapsack(vector<Unit *> units, int maxScore);
    Unit *cloneUnit(Unit *unit);
    void updateScoreforAttack(Army *enemy);
    void updateScoreforDefense(Army *enemy);

  public:
    LiberationArmy(Unit **unitArray, int size, string name,
                   BattleField *battleField);
    void fight(Army *enemy, bool defense) override;
    string str() const override;
};

//!-----------------------------------------------------
//! CLASS ARVN
//!-----------------------------------------------------
class ARVN : public Army {
  private:
    int nearestFibonacci(int value);
    vector<Unit *> knapsack(vector<Unit *> units, int maxScore);
    Unit *cloneUnit(Unit *unit);
    void updateScoreforAttack(Army *enemy);
    void updateScoreforDefense(Army *enemy);

  public:
    ARVN(Unit **unitArray, int size, string name, BattleField *battleField);
    void fight(Army *enemy, bool defense = false) override;
    string str() const;
};

//!-----------------------------------------------------
//! CLASS TerrainElement và các lớp dẫn xuất
//!-----------------------------------------------------
class TerrainElement {
  protected:
    Position pos;
    double calculateDistance(const Position &pos1, const Position &pos2) {
        double rowDiff = pos1.getRow() - pos2.getRow();
        double colDiff = pos1.getCol() - pos2.getCol();

        return (sqrt(rowDiff * rowDiff + colDiff * colDiff));
    }

  public:
    TerrainElement(Position);
    virtual ~TerrainElement();
    virtual void getEffect(Army *army) = 0;
};

class Road : public TerrainElement {
  public:
    Road(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
};

class Mountain : public TerrainElement {
  public:
    Mountain(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
};

class River : public TerrainElement {
  public:
    River(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
};

class Urban : public TerrainElement {
  public:
    Urban(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
};

class Fortification : public TerrainElement {
  public:
    Fortification(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
};

class SpecialZone : public TerrainElement {
  public:
    SpecialZone(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
};

//!-----------------------------------------------------
//! CLASS BattleField
//!-----------------------------------------------------
class BattleField {
  private:
    int n_rows, n_cols;
    // 2D mảng chứa con trỏ đến các đối tượng TerrainElement
    TerrainElement ***terrain;
    bool checkValidPos(Position *pos) const;

  public:
    // Constructor nhận các vector vị trí của các yếu tố địa hình
    BattleField(int n_rows, int n_cols, const vector<Position *> &arrayForest,
                const vector<Position *> &arrayRiver,
                const vector<Position *> &arrayFortification,
                const vector<Position *> &arrayUrban,
                const vector<Position *> &arraySpecialZone);
    ~BattleField();
    TerrainElement *getElement(int r, int c) const;
    string str() const;
};

//!-----------------------------------------------------
//! CLASS Configuration
//!-----------------------------------------------------
class Configuration {
  private:
    int num_rows;
    int num_cols;
    vector<Position *> arrayForest;
    vector<Position *> arrayRiver;
    vector<Position *> arrayFortification;
    vector<Position *> arrayUrban;
    vector<Position *> arraySpecialZone;
    Unit *liberationUnits[100];
    int numLiberationUnits;
    Unit *ARVNUnits[100];
    int numARVNUnits;

    int eventCode;
    void replaceChars(string &line) {
        for (char &c : line) {
            if (c == '[' || c == ']' || c == '(' || c == ')' || c == ',' ||
                c == '=' || c == ':') {
                c = ' ';
            }
        }
    }

    string createOutput(const vector<Position *> &A) const {
        ostringstream oss;
        int size = A.size();

        for (int i = 0; i < size; ++i) {
            oss << A[i]->str();
            if (i != size - 1)
                oss << ',';
        }

        return oss.str();
    }

    string createOutput(Unit* const* B) const {
        ostringstream oss;

        for (int i = 0; i < 100; ++i) {
            if (B[i] == nullptr)
              break;
            oss << B[i]->str();
            if (B[i + 1] != nullptr)
                oss << ',';
        }

        return oss.str();
    }

  public:
    Configuration(const string &filepath);
    ~Configuration();
    string str() const;

    int getNumRows() const { return num_rows; }
    int getNumCols() const { return num_cols; }
    const vector<Position *> &getForestPositions() const { return arrayForest; }
    const vector<Position *> &getRiverPositions() const { return arrayRiver; }
    const vector<Position *> &getFortificationPositions() const {
        return arrayFortification;
    }
    const vector<Position *> &getUrbanPositions() const { return arrayUrban; }
    const vector<Position *> &getSpecialZonePositions() const {
        return arraySpecialZone;
    }
    int getEventCode() const { return eventCode; }
    Unit **getLiberationUnits() { return liberationUnits; }
    int getNumLiberationUnits() { return numLiberationUnits; }
    Unit **getARVNUnits() { return ARVNUnits; }
    int getNumARVNUnits() { return numARVNUnits; }
};

//!-----------------------------------------------------
//! Lớp HCMCampaign
//!-----------------------------------------------------
class HCMCampaign {
  private:
    Configuration *config;
    BattleField *battleField;
    LiberationArmy *liberationArmy;
    ARVN *ARVNarmy;

    Unit **createData(Unit** A, int size) {
        Unit **newArray = new Unit *[size];

        for (int i = 0; i < size; ++i) {
            newArray[i] = A[i];
        }

        return newArray;
    }

    void UpdateScore(Army *army);

  public:
    HCMCampaign(const string &config_file_path);
    ~HCMCampaign() {
        delete config;
        delete battleField;
        delete liberationArmy;
        delete ARVNarmy;
    }
    void run();
    string printResult();
};

#endif
