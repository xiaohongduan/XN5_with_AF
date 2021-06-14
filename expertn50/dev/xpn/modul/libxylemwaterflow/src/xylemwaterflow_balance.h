// contains some variables for single trees to make the water balance

class xylemwaterflow_balance
{
public:
    // daily
    double uptake_d;           // [l] = [mm^3]*1e-6
    double waterStorage_d;     // [l] = [mm^3]*1e-6
    double transpiration_d;    // [l] = [mm^3]*1e-6
    double transpiration_pot_d;    // [l] = [mm^3]*1e-6
    double photosynthesis_d;    
    double photosynthesis_pot_d;   
    double deltaStorage_d;     // [l] = [mm^3]*1e-6
    double waterStorage_old_d; // [l] = [mm^3]*1e-6
    double flow_dbh_d;         // [l] = [mm^3]*1e-6
    double min_pot_stem; // Mpa
    double max_pot_stem; // Mpa
    // hourly
    double uptake_h;           // [l] = [mm^3]*1e-6
    double waterStorage_h;     // [l] = [mm^3]*1e-6
    double transpiration_h;    // [l] = [mm^3]*1e-6
    double transpiration_pot_h;    // [l] = [mm^3]*1e-6
    double photosynthesis_h;    
    double photosynthesis_pot_h;   
    double deltaStorage_h;     // [l] = [mm^3]*1e-6
    double waterStorage_old_h; // [l] = [mm^3]*1e-6
    double flow_dbh_h;         // [l] = [mm^3]*1e-6
    double sum_d;
    double sum_h;
    double pot_stem; // Mpa
    xylemwaterflow_balance()
    {
        uptake_d = 0.0;           // [l] = [mm^3]*1e-6
        waterStorage_d = 0.0;     // [l] = [mm^3]*1e-6
        transpiration_d = 0.0;    // [l] = [mm^3]*1e-6
        transpiration_pot_d = 0.0;
        photosynthesis_d = 0.0;    // [l] = [mm^3]*1e-6
        photosynthesis_pot_d = 0.0;
        deltaStorage_d = 0.0;     // [l] = [mm^3]*1e-6
        waterStorage_old_d = 0.0; // [l] = [mm^3]*1e-6
        flow_dbh_d = 0.0;         // [l] = [mm^3]*1e-6
        min_pot_stem = 0.0; // Mpa
        max_pot_stem = 0.0; // Mpa
        // hourly
        uptake_h = 0.0;           // [l] = [mm^3]*1e-6
        waterStorage_h = 0.0;     // [l] = [mm^3]*1e-6
        transpiration_h = 0.0;    // [l] = [mm^3]*1e-6
        transpiration_pot_h = 0.0;
        photosynthesis_h = 0.0;    // [l] = [mm^3]*1e-6
        photosynthesis_pot_h = 0.0;
        deltaStorage_h = 0.0;     // [l] = [mm^3]*1e-6
        waterStorage_old_h = 0.0; // [l] = [mm^3]*1e-6
        flow_dbh_h = 0.0;         // [l] = [mm^3]*1e-6
        sum_d = 0.0;
        sum_h = 0.0;
        pot_stem = 0.0; // Mpa
    };
    void reset_d();
    void reset_h();
};
