# SIRD-V Epidemic Model
A C++ implementation of the SIRD-V mathematical model for epidemic simulation.

## Population Compartments
- **S**: Susceptible
- **I**: Infected
- **R**: Recovered
- **D**: Deceased
- **V**: Vaccinated

## Basic Reproduction Number (R0)
The model calculates the transmission potential using:
$$R0 = \frac{\beta}{\gamma + \mu}$$