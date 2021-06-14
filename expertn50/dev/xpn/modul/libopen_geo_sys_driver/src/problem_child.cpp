#include "problem_child.h"

#if defined (USE_MPI)
#include <mpi.h>
#endif

#if defined(USE_MPI_REGSOIL)
#include "par_ddc.h"
#endif
#include <cfloat>
#include <iostream>
#include <sstream>
//WW
//
/*------------------------------------------------------------------------*/
/* Pre-processor definitions */
#include "makros.h"
/*------------------------------------------------------------------------*/
// MSHLib
#include "msh_lib.h"
#include "msh_node.h"

/*------------------------------------------------------------------------*/
// Data file
//OK411
extern int ReadData(char*, GEOLIB::GEOObjects& geo_obj, std::string& unique_name);
/* PCS */
#include "pcs_dm.h"
#include "rf_pcs.h"
//16.12.2008.WW #include "rf_apl.h"
#include "par_ddc.h"
#include "rf_react.h"
#include "rf_st_new.h"
#include "rf_tim_new.h"
#include "rfmat_cp.h"
//#include "rf_vel_new.h"
#include "rf_fluid_momentum.h"
#include "rf_random_walk.h"
// Finite element
#include "DUMUX.h"                                // BG 02/2011
#include "Eclipse.h"                              //BG 09/2009
#include "Output.h"
#include "fem_ele_std.h"
#include "files0.h"                               // GetLineFromFile1
#include "rf_bc_new.h"
#include "rf_node.h"
#include "rf_out_new.h"
#include "tools.h"
//
#ifdef CHEMAPP
#include "eqlink.h"
#endif
#ifdef UDE_REACT
#include "rf_REACT_ude.h"
#endif
#ifdef GEM_REACT
#include "rf_REACT_GEM.h"
#endif
#ifdef BRNS
// BRNS dll link; HB 02.11.2007
#include "rf_REACT_BRNS.h"
#endif
#include "rf_kinreact.h"

problem_child::problem_child(char* filename):Problem(filename)
{
}

problem_child::~problem_child(void)
{
}

/**************************************************************************
   FEMLib-Method:
   07/2008 WW Implementation
   01/2009 WW Update
   03/2012 JT Many changes. Allow independent time stepping.
**************************************************************************/
int problem_child::Euler_TimeDiscretize_TimeStep()
{
    long accepted_times = 0;
    long rejected_times = 0;
    double dt_rec;
    int i;
    bool force_output;
    last_dt_accepted = false; // JT: false first. Thus copy node values after first dt.
    //
    CTimeDiscretization* m_tim = NULL;
    aktueller_zeitschritt = 0;
     
    //ScreenMessage("\n\n***Start time steps\n");
    //
    // Output zero time initial values
#if defined(USE_MPI)
        if(myrank == 0)
        {
#endif
//  OUTData(0.0,aktueller_zeitschritt,true);
#if defined(USE_MPI)
        }
#endif
    
    //
    // ------------------------------------------
    // PERFORM TRANSIENT SIMULATION
    // ------------------------------------------
//  while(end_time > current_time)
    {
        // Get time step
        dt = dt_rec = DBL_MAX;
        for(i=0; i<(int)active_process_index.size(); i++)
        {
            m_tim = total_processes[active_process_index[i]]->GetTimeStepping();
            if(!m_tim->time_active) continue; // JT
            dt = MMin(dt,m_tim->CalcTimeStep(current_time));
            dt_rec = MMin(dt_rec,m_tim->recommended_time_step); // to know if we have a critical time alteration
        }
        SetTimeActiveProcesses(); // JT2012: Activate or deactivate processes with independent time stepping
//
#if defined(USE_MPI)
        MPI_Bcast(&dt, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); // all processes use the same time stepping (JT->WW. Must they always?)
#endif
//
        // Update time settings
        aktueller_zeitschritt++;
        current_time += dt;
        aktuelle_zeit = current_time;
        //
        // Print messsage
#if defined(USE_MPI)
        if(myrank == 0)
        {
#endif
        std::cout << "\n\n#############################################################\n";
        std::cout << "Time step: " << aktueller_zeitschritt << "|  Time: " <<
        current_time << "|  Time step size: " << dt << std::endl;
        if(dt_rec > dt){
            std::cout << "This time step size was modified to match a critical time!" << std::endl;
        }
#if defined(USE_MPI)
        }
#endif
        if(CouplingLoop())
        {
            // ---------------------------------
            // TIME STEP ACCEPTED
            // ---------------------------------
            last_dt_accepted = true;
            ScreenMessage("This step is accepted.\n");
            PostCouplingLoop();
            if(print_result)
            {
                if(current_time < end_time)
                    force_output = false;
                else // JT: Make sure we printout on last time step
                    force_output = true;
#if defined(USE_MPI)
                if(myrank == 0)
                {
#endif
                    //
                OUTData(current_time, aktueller_zeitschritt,force_output);
#if defined(USE_MPI)
                }
#endif
            }
            accepted_times++;
            for(i=0; i<(int)active_process_index.size(); i++)
            {
                m_tim = total_processes[active_process_index[i]]->GetTimeStepping();
                if(m_tim->time_active) m_tim->accepted_step_count++;
            }
        }
        else
        {
            // ---------------------------------
            // TIME STEP FAILED
            // ---------------------------------
            last_dt_accepted = false;
            ScreenMessage("This step is rejected: Redo, with a new time step.\n");
            rejected_times++;
            current_time -= dt;
            aktuelle_zeit = current_time;
            aktueller_zeitschritt--;
            //
            // decrement active dt, and increment count
            for(i=0; i<(int)active_process_index.size(); i++)
            {
                m_tim = total_processes[active_process_index[i]]->GetTimeStepping();
                if(!m_tim->time_active)
                    continue;
                m_tim->rejected_step_count++;
                m_tim->last_active_time -= dt;
                //
                // Copy nodal values in reverse
                if(isDeformationProcess(total_processes[active_process_index[i]]->getProcessType()))
                    continue;
                total_processes[active_process_index[i]]->CopyTimestepNODValues(false);
                // JT: This wasn't done before. Is it needed? // total_processes[active_process_index[i]]->CopyTimestepELEValues(false);
            }
        }
        ScreenMessage("\n#############################################################\n");
        if(aktueller_zeitschritt >= max_time_steps)
            return 1;

//      // executing only one time step for profiling
//      current_time = end_time;
    }

/*#if defined(USE_MPI) // JT2012
    if(myrank == 0)
    {
#endif
    std::cout << "\n----------------------------------------------------\n";
    for(i=0; i<(int)active_process_index.size(); i++) // JT2012
    {
        m_tim = total_processes[active_process_index[i]]->Tim;
        std::cout << "\nFor process: " << convertProcessTypeToString(total_processes[active_process_index[i]]->getProcessType()) << std::endl;
        if(m_tim->time_control_name == "NONE"){
            std::cout << "No time control for this process." << std::endl;
        }
        else{
            std::cout << "Accepted time steps:                " << m_tim->accepted_step_count << std::endl;
            std::cout << "Rejected time steps:                " << m_tim->rejected_step_count << std::endl;
        }
        if(total_processes[active_process_index[i]]->m_num->nls_max_iterations > 1){
            std::cout << "Number of non-converged iterations: " << total_processes[active_process_index[i]]->num_notsatisfied << std::endl;
            std::cout << "Number of stagnated iterations:     " << total_processes[active_process_index[i]]->num_diverged << std::endl;
        }
    }
    std::cout<<"\n----------------------------------------------------\n";*/
#if defined(USE_MPI)
    }
#endif
return 0;
}
