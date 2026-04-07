#include <iostream>
#include <string>
using namespace std;

class Device {
protected:
    string name;
    bool isOn;
    double powerWatts;
    double hoursPerDay;

public:
    Device(string n, double pW, double hpd)
        : name(n), isOn(false), powerWatts(pW), hoursPerDay(hpd) {}

    virtual ~Device() {}

    void turnOn()  { isOn = true; }
    void turnOff() { isOn = false; }

    bool getStatus() const { return isOn; }
    string getName() const { return name; }

    virtual double getDailyEnergyKWh() const {
        double factor = isOn ? 1.0 : 0.1;
        return (powerWatts / 1000.0) * hoursPerDay * factor;
    }

    virtual void showInfo() const {
        cout << "Device: " << name
             << " | On: " << (isOn ? "Yes" : "No")
             << " | Power: " << powerWatts << " W"
             << " | Hours/day: " << hoursPerDay << endl;
    }
};

class Light : public Device {
    int brightness;

public:
    Light(string n, double pW, double hpd, int b = 100)
        : Device(n, pW, hpd), brightness(b) {}

    void setBrightness(int b) {
        if (b < 0) b = 0;
        if (b > 100) b = 100;
        brightness = b;
    }

    int getBrightness() const { return brightness; }

    double getDailyEnergyKWh() const override {
        double factor = isOn ? (brightness / 100.0) : 0.05;
        return (powerWatts / 1000.0) * hoursPerDay * factor;
    }

    void showInfo() const override {
        cout << "[Light] ";
        Device::showInfo();
        cout << "   Brightness: " << brightness << " %" << endl;
    }
};

class AirConditioner : public Device {
    double setTemp;
    bool ecoMode;

public:
    AirConditioner(string n, double pW, double hpd, double temp = 24.0)
        : Device(n, pW, hpd), setTemp(temp), ecoMode(false) {}

    void setTemperature(double t) { setTemp = t; }
    double getTemperature() const { return setTemp; }

    void setEcoMode(bool e) { ecoMode = e; }
    bool getEcoMode() const { return ecoMode; }

    double getDailyEnergyKWh() const override {
        double tempFactor = 24.0 / setTemp;
        double ecoFactor = ecoMode ? 0.8 : 1.0;
        double factor = isOn ? (tempFactor * ecoFactor) : 0.1;
        return (powerWatts / 1000.0) * hoursPerDay * factor;
    }

    void showInfo() const override {
        cout << "[AC] ";
        Device::showInfo();
        cout << "   Temp: " << setTemp
             << " C | Eco: " << (ecoMode ? "On" : "Off") << endl;
    }
};

class WashingMachine : public Device {
    bool scheduled;
    int startHour;

public:
    WashingMachine(string n, double pW, double hpd)
        : Device(n, pW, hpd), scheduled(false), startHour(22) {}

    void schedule(int hour) {
        if (hour < 0) hour = 0;
        if (hour > 23) hour = 23;
        scheduled = true;
        startHour = hour;
    }

    bool isScheduled() const { return scheduled; }
    int getStartHour() const { return startHour; }

    double getDailyEnergyKWh() const override {
        return (powerWatts / 1000.0) * hoursPerDay;
    }

    void showInfo() const override {
        cout << "[WashingMachine] ";
        Device::showInfo();
        if (scheduled)
            cout << "   Scheduled at: " << startHour << ":00" << endl;
        else
            cout << "   Not scheduled" << endl;
    }
};

class Room {
    string name;
    Device* devices[4];
    int deviceCount;

public:
    Room(string n) : name(n), deviceCount(0) {
        for (int i = 0; i < 4; ++i) {
            devices[i] = nullptr;
        }
    }

    ~Room() {
        for (int i = 0; i < deviceCount; ++i) {
            delete devices[i];
        }
    }

    string getName() const { return name; }

    void addDevice(Device* d) {
        if (deviceCount < 4) {
            devices[deviceCount++] = d;
        } else {
            cout << "Cannot add more than 4 devices to this room." << endl;
        }
    }

    Device** getDevices() { return devices; }
    int getDeviceCount() const { return deviceCount; }

    double getDailyEnergyKWh() const {
        double sum = 0.0;
        for (int i = 0; i < deviceCount; ++i) {
            sum += devices[i]->getDailyEnergyKWh();
        }
        return sum;
    }

    void showRoomInfo() const {
        cout << "Room: " << name << endl;
        for (int i = 0; i < deviceCount; ++i) {
            devices[i]->showInfo();
        }
        cout << "   Room daily energy: " << getDailyEnergyKWh() << " kWh" << endl;
    }
};

class SmartHome {
    Room* rooms[3];
    int roomCount;
    double peakRate;
    double offPeakRate;
    int currentHour;

public:
    SmartHome(double peak, double offPeak)
        : peakRate(peak), offPeakRate(offPeak), currentHour(19), roomCount(0) {
        for (int i = 0; i < 3; ++i) {
            rooms[i] = nullptr;
        }
    }

    ~SmartHome() {
        for (int i = 0; i < roomCount; ++i) {
            delete rooms[i];
        }
    }

    void addRoom(Room* r) {
        if (roomCount < 3) {
            rooms[roomCount++] = r;
        } else {
            cout << "Cannot add more than 3 rooms." << endl;
        }
    }

    void setCurrentHour(int h) {
        if (h < 0) h = 0;
        if (h > 23) h = 23;
        currentHour = h;
    }

    int getCurrentHour() const { return currentHour; }

    bool isPeakHour() const {
        return currentHour >= 17 && currentHour <= 22;
    }

    double getTotalDailyEnergyKWh() const {
        double sum = 0.0;
        for (int i = 0; i < roomCount; ++i) {
            sum += rooms[i]->getDailyEnergyKWh();
        }
        return sum;
    }

    double estimateDailyCost() const {
        double total = getTotalDailyEnergyKWh();
        double peakEnergy = total * 0.5;
        double offPeakEnergy = total * 0.5;
        return peakEnergy * peakRate + offPeakEnergy * offPeakRate;
    }

    void showHomeStatus() const {
        cout << "\n===== SMART HOME STATUS =====" << endl;
        cout << "Current hour: " << currentHour
             << " -> " << (isPeakHour() ? "PEAK" : "OFF-PEAK") << endl;
        for (int i = 0; i < roomCount; ++i) {
            rooms[i]->showRoomInfo();
        }
        cout << "Total daily energy: " << getTotalDailyEnergyKWh() << " kWh" << endl;
        cout << "Estimated daily cost: " << estimateDailyCost()
             << " rupees" << endl;
    }

    void optimizeEnergy() {
        cout << "\n>>> Running energy optimization...\n";

        for (int i = 0; i < roomCount; ++i) {
            Room* r = rooms[i];
            for (int j = 0; j < r->getDeviceCount(); ++j) {
                Device* d = r->getDevices()[j];

                Light* light = dynamic_cast<Light*>(d);
                if (light && isPeakHour() && light->getStatus()) {
                    if (light->getBrightness() <= 30) {
                        cout << "Turning OFF low-brightness light '"
                             << light->getName()
                             << "' in " << r->getName() << endl;
                        light->turnOff();
                    }
                }

                AirConditioner* ac = dynamic_cast<AirConditioner*>(d);
                if (ac && isPeakHour() && ac->getStatus()) {
                    ac->setEcoMode(true);
                    if (ac->getTemperature() < 24.0) {
                        cout << "Raising AC temperature of '"
                             << ac->getName()
                             << "' in " << r->getName()
                             << " to 24 C (eco mode)" << endl;
                        ac->setTemperature(24.0);
                    }
                }

                WashingMachine* wm = dynamic_cast<WashingMachine*>(d);
                if (wm && isPeakHour()) {
                    cout << "Scheduling washing machine '"
                         << wm->getName()
                         << "' to 23:00 (off-peak)" << endl;
                    wm->schedule(23);
                }
            }
        }

        cout << ">>> Optimization completed.\n";
    }
};

void setupDemoHome(SmartHome& home) {
    Room* living = new Room("Living Room");
    Room* bedroom = new Room("Bedroom");
    Room* laundry = new Room("Laundry");

    Light* livingLight = new Light("Living Main Light", 18.0, 6.0, 40);
    AirConditioner* livingAC = new AirConditioner("Living AC", 1200.0, 8.0, 22.0);
    livingLight->turnOn();
    livingAC->turnOn();
    living->addDevice(livingLight);
    living->addDevice(livingAC);

    Light* bedLight = new Light("Bedroom Lamp", 10.0, 5.0, 20);
    AirConditioner* bedAC = new AirConditioner("Bedroom AC", 900.0, 7.0, 23.0);
    bedLight->turnOn();
    bedAC->turnOn();
    bedroom->addDevice(bedLight);
    bedroom->addDevice(bedAC);

    WashingMachine* wm = new WashingMachine("Washer", 1500.0, 1.5);
    wm->turnOn();
    laundry->addDevice(wm);

    home.addRoom(living);
    home.addRoom(bedroom);
    home.addRoom(laundry);
}

void showMenu() {
    cout << "\n===== ENERGY CONSUMPTION OPTIMIZER =====" << endl;
    cout << "1. Show home status" << endl;
    cout << "2. Set current hour" << endl;
    cout << "3. Run optimization" << endl;
    cout << "4. Exit" << endl;
    cout << "Enter choice: ";
}

int main() {
    SmartHome home(0.30, 0.15);
    setupDemoHome(home);

    int choice = 0;
    while (choice != 4) {
        showMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            home.showHomeStatus();
            break;
        case 2: {
            int h;
            cout << "Enter current hour (0–23): ";
            cin >> h;
            home.setCurrentHour(h);
            break;
        }
        case 3:
            home.optimizeEnergy();
            home.showHomeStatus();
            break;
        case 4:
            cout << "Exiting program..." << endl;
            break;
        default:
            cout << "Invalid choice. Try again." << endl;
        }
    }

    return 0;
}