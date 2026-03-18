#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

/*
===========================================================
Epidemic Dynamic Engine (SIRD-V Model)

Population Compartments:
S - Susceptible
I - Infected
R - Recovered
D - Dead
V - Vaccinated

Epidemiological Parameters:
beta   - Infection rate
gamma  - Recovery rate
mu     - Death rate

R0 = beta / (gamma + mu)

===========================================================
*/

class EpidemicEngine {
    
private:
    // Population groups
    double S, I, R, D, V;

    // Epidemiological parameters
    double beta, gamma, mu;

    // Vaccination
    double vaccinationRate;
    double vaccineEffectiveness;

    // Lockdown
    double originalBeta;
    int lockdownDay;
    double lockdownReduction;

    int N; // Total population

    // Peak infection analysis
    double peakInfected;
    int peakDay;

    // Infection history for graph
    vector<double> infectedHistory;

public:
    // Constructor
    EpidemicEngine(int population, int initialInfected,
                   double b, double g, double m,
                   double vRate, double vEffect,
                   int lDay, double lReduction)
    {
        N = population;
        I = initialInfected;
        S = population - initialInfected;
        R = D = V = 0;

        beta = b;
        gamma = g;
        mu = m;

        vaccinationRate = vRate;
        vaccineEffectiveness = vEffect;

        lockdownDay = lDay;
        lockdownReduction = lReduction;
        originalBeta = beta;

        peakInfected = I;
        peakDay = 0;
    }


    void update(int day) {
    // Apply lockdown if enabled
    if (day == lockdownDay) {
        beta = originalBeta * (1 - lockdownReduction);
    }

    // Calculate new transitions
    double newInfected = beta * S * I / N;     // The formula: Each susceptible person has a chance beta of catching infection proportional to the number of infected people.
    double newRecovered = gamma * I;        // recovery rate * current infected people
    double newDeaths = mu * I;             // death rate * current infected people

    double maxVaccinations = min(vaccinationRate, S - newInfected); // You don't vaccinate more people than available.
    if (maxVaccinations < 0) maxVaccinations = 0; // safety check

    double effectiveVaccinated = maxVaccinations * vaccineEffectiveness; // not all vaccines are effective so only these people are removed from susceptible pool.
    // Clamp transitions FIRST
    newInfected = std::min(newInfected, S);
    newRecovered = std::min(newRecovered, I);
    newDeaths = std::min(newDeaths, I - newRecovered);
    // totalLeavingS = people leaving susceptible pool today (getting infected + vaccinated).
    double totalLeavingS = newInfected + effectiveVaccinated;
    // if more people are leaving than actually susceptible which is impossible, we scale both values down proportionally.
    if (totalLeavingS > S) {
        double scale = S / totalLeavingS;
        newInfected *= scale;
        effectiveVaccinated *= scale;
    }

    // Update compartments
    S -= (newInfected + effectiveVaccinated);
    V += effectiveVaccinated;
    I += (newInfected - newRecovered - newDeaths);
    R += newRecovered;
    D += newDeaths;

    // Clamp negative values to zero
    // since negative means imaginary people
    if (S < 0) S = 0;
    if (I < 0) I = 0;
    if (R < 0) R = 0;
    if (V < 0) V = 0;
    if (D < 0) D = 0;

    // Store history for graph
    infectedHistory.push_back(I);

    // Track peak infection keeps maximum infected population seen so far
    if (I > peakInfected) {
        peakInfected = I;
        peakDay = day;
    }
}

    // Display daily table row
    void display(int day) {
        cout << setw(3) << day << "  "
             << setw(9) << (int)S << "  "
             << setw(9) << (int)I << "  "
             << setw(9) << (int)R << "  "
             << setw(9) << (int)D << "  "
             << setw(9) << (int)V << endl;
    }

    // Calculate R0
    double calculateR0() {
        return beta / (gamma + mu);
    }

    // Display parameters
    void displayParameters() {
        cout << "\nEpidemiological Parameters\n";
        cout << "--------------------------\n";
        cout << "Beta   : " << beta << endl;
        cout << "Gamma  : " << gamma << endl;
        cout << "Mu     : " << mu << endl;
        cout << "R0     : " << calculateR0() << endl;
    }

    // Explain R0
    void explainR0() {
        double R0 = calculateR0();

        cout << "\nR0 Explanation\n";
        cout << "--------------\n";
        cout << "R0 = β / (γ + μ) = " << R0 << endl;

        if (R0 > 1)
            cout << "Epidemic will SPREAD\n";
        else if (R0 == 1)
            cout << "Epidemic is STABLE\n";
        else
            cout << "Epidemic will DIE OUT\n";
    }

    // Display effective R
    void displayEffectiveR() {
        cout << "\nReproduction Numbers\n";
        cout << "-------------------\n";
        cout << "Basic R0 (Before Lockdown): "
             << originalBeta / (gamma + mu) << endl;
        cout << "Effective Re (After Lockdown): "
             << beta / (gamma + mu) << endl;
    }

    // Peak infection
    void showPeak() {
        cout << "\nPeak Infection Details\n";
        cout << "----------------------\n";
        cout << "Day of Maximum Infection : Day " << peakDay << endl;
        cout << "Maximum Infected People  : " << (int)peakInfected << endl;
    }

    // ASCII graph
    void showFinalGraph() {
        cout << "\nInfection Graph\n";
        cout << "---------------\n";
        for (size_t d = 0; d < infectedHistory.size(); d++) {
            int bars = (int)((infectedHistory[d] / peakInfected) * 50);
            cout << "Day " << setw(3) << d << " | ";
            for (int i = 0; i < bars; i++) cout << "#";
            cout << endl;
        }
    }

    // Display final totals
    void showFinalTotals() {
        cout << "\nFINAL POPULATION SUMMARY\n";
        cout << "------------------------\n";
        cout << "Susceptible  : " << (int)S << endl;
        cout << "Infected     : " << (int)I << endl;
        cout << "Recovered    : " << (int)R << endl;
        cout << "Dead         : " << (int)D << endl;
        cout << "Vaccinated   : " << (int)V << endl;

        cout << "------------------------\n";
        cout << "Total        : " << (int)(S + I + R + D + V) << endl;
    }

    // Getters for CSV
    double getS() { return S; }
    double getI() { return I; }
    double getR() { return R; }
    double getD() { return D; }
    double getV() { return V; }
};

int main() {
    int population, initialInfected, lockdownDay, days;
    double beta, gamma, mu;
    double vaccinationRate, vaccineEffectiveness, lockdownReduction;

    // Input
    cout << "Enter total population: ";
    cin >> population;
    cout << "Enter initial infected: ";
    cin >> initialInfected;
    cout << "Enter beta (infection rate): ";
    cin >> beta;
    cout << "Enter gamma (recovery rate): ";
    cin >> gamma;
    cout << "Enter mu (death rate): ";
    cin >> mu;
    
    if(initialInfected > population){
    cout<<"ERROR : Initial infected cannot exceed population ! \n ";
    return 1;
    }

    // Validate epidemiological rates
    if (gamma + mu > 1.0) {
    cout << "\nERROR: Invalid epidemiological parameters!\n";
    cout << "Reason: gamma + mu must be <= 1\n";
    cout << "You entered gamma + mu = " << (gamma + mu) << endl;
    cout << "This would remove more than 100% of infected people per day.\n";
    cout << "Please restart and enter valid values.\n";
    return 1 ;
}

    if (mu > gamma) {
    cout << "\nWARNING: Death rate  is higher than recovery rate .\n";
    cout << "This represents a highly lethal disease scenario.\n";
}

// input for vaccination and its effectiveness
    cout << "Enter vaccinations per day: ";
    cin >> vaccinationRate;
    cout << "Enter vaccine effectiveness (0-1): ";
    cin >> vaccineEffectiveness;

//death recovery infection rate cannot be negative
if (beta < 0 || gamma < 0 || mu < 0) {
        cout << "ERROR: Rates cannot be negative!\n";
        return 1;
    }

    // vaccination effectiveness must be between 0 and 1
    if (vaccineEffectiveness < 0 || vaccineEffectiveness > 1) {
        cout << "ERROR: Vaccine effectiveness must be between 0 and 1!\n";
        return 1;
    }
    
    // Lockdown choice
    char lockdownChoice;
    cout << "Do you want to enable lockdown? (for yes enter y / for no enter n ): ";
    cin >> lockdownChoice;

    if (lockdownChoice == 'y' || lockdownChoice == 'Y') {
        cout << "Enter lockdown start day: ";
        cin >> lockdownDay;
        cout << "Enter lockdown reduction (0-1): ";
        cin >> lockdownReduction;
        if(lockdownReduction < 0 || lockdownReduction > 1){
        cout<<"ERROR : lockdown Reduction must be  between 0 and 1 ! \n";
        return 1;}
    } 
    else {
        lockdownDay = -1;       // No lockdown
        lockdownReduction = 0;  // No effect
    }

    cout << "Enter simulation days: ";
    cin >> days;

    // Create model
    EpidemicEngine model(population, initialInfected,
                          beta, gamma, mu,
                          vaccinationRate, vaccineEffectiveness,
                          lockdownDay, lockdownReduction);

    // Display parameters and R0
    model.displayParameters();
    model.explainR0();
    model.displayEffectiveR();

    // CSV output
    ofstream file("epidemic_data.csv");
    file << "Day    ,Susceptible,     Infected,    Recovered,    Dead,    Vaccinated\n";

    // Table header
    cout << "\nDay     S(Sus)       I(Inf)     R(Rec)      D(Dead)     V(Vacc)\n";
    cout << "---------------------------------------------------------------------\n";

    // Simulation loop
    for (int day = 0; day <= days; day++) {
        model.display(day);

        file << day << ","
             << model.getS() << ","
             << model.getI() << ","
             << model.getR() << ","
             << model.getD() << ","
             << model.getV() << "\n";

        model.update(day);
    }

    file.close();

   // Results
    model.showPeak();
    model.showFinalGraph();
    model.showFinalTotals();

    cout << "\nSimulation complete. Data saved to epidemic_data.csv\n";
    return 0;
}