#include <stdio.h>
#include <cassert>
#include <string.h>
#include <inttypes.h>
#include <cmath>

using namespace std;

#include "cbp3_def.h"
#include "cbp3_framework.h"

#define default_counter_state SaturatingCounter::weakly_taken

class SaturatingCounter{
    public: 
    enum State {
        strongly_not_taken,
        weakly_not_taken,
        weakly_taken,
        strongly_taken
    };
    SaturatingCounter();
    SaturatingCounter(State);
    bool should_take();
    void taken(bool was_taken);
    State get_state();
    
    private:
    State m_state;
};

SaturatingCounter::SaturatingCounter(){
    SaturatingCounter(default_counter_state);
}

SaturatingCounter::SaturatingCounter(SaturatingCounter::State starting_state){
    m_state = starting_state;
}

bool SaturatingCounter::should_take(){
    return m_state == weakly_taken || m_state == strongly_taken;
}

void SaturatingCounter::taken(bool was_taken){
    switch (m_state){
    case strongly_not_taken:
        m_state = was_taken ? weakly_not_taken : strongly_not_taken;
        break;
    case weakly_not_taken:
        m_state = was_taken ? weakly_taken : strongly_not_taken;
        break;
    case weakly_taken:
        m_state = was_taken ? strongly_taken : weakly_not_taken;
        break;
    case strongly_taken:
        m_state = was_taken ? strongly_taken : weakly_taken;
        break;
    }
}

SaturatingCounter::State SaturatingCounter::get_state(){
    return m_state;
}

class Gshare{
    public:
    Gshare(SaturatingCounter::State default_state, int addr_width, uint32_t default_history);
    ~Gshare();
    bool should_take(uint32_t address);
    void taken(bool was_taken, uint32_t address);

    private:
    SaturatingCounter* m_counter_table;
    int m_addr_width;
    uint64_t m_history;
};

Gshare::Gshare(SaturatingCounter::State default_state, int addr_width, uint32_t default_history){
    int table_size = pow(2, addr_width);
    
    m_addr_width = addr_width >= 32 ? 31 : addr_width;
    m_history = default_history;

    m_counter_table = new SaturatingCounter[table_size];

    for (int i = 0; i < table_size; i++){
        m_counter_table[i] = SaturatingCounter(default_state);
    }
}

Gshare::~Gshare(){
    delete[] m_counter_table;
}

bool Gshare::should_take(uint32_t address){
    uint32_t table_idx = address ^ m_history;
    table_idx &= (1U << m_addr_width) - 1;    // Mask to only keep lower n bits of table_idx

    return m_counter_table[table_idx].should_take();
}

void Gshare::taken(bool was_taken, uint32_t address){
    uint32_t table_idx = address ^ m_history;
    table_idx &= (1U << m_addr_width) - 1;   // Mask to only keep lower n bits of table_idx

    m_counter_table[table_idx].taken(was_taken);

    m_history = (m_history << 1) | was_taken;
}

/*
# Seperate Predictor Types

In each function (mainly PredictorReset() and PredictorRunACycle()
there will be a if-else statement that switches between the different
types of predictor. Code specific to each type of predictor should
be implemented in the branch of the if-else statement corresponding
to the specific type of branch predictor. In general,

- the predictor will use saturating-counter in the first run,
- the gselect predictor will be used in the second run, and
- the gshare predictor will be used in the third run.
*/
#define TWO_BIT_PREDICTOR_ 0U
#define GSELECT_PREDICTOR_ 1U
#define GSHARE_PREDICTOR_  2U

/*
# Two branch history registers

the framework provids real branch results at fetch stage to simplify branch history
update for predicting later branches. however, they are not available until execution stage
in a real machine. therefore, you can only use them to update predictors at or after the
branch is executed.
*/

// cost: depending on predictor size
uint32_t brh_fetch;
uint32_t brh_retire;

// Count number of runs, this is used to switch between
// the different predictors.
uint32_t runs;

// This function runs ONCE when the simulation starts. Globals
// State should be initialized here, if there is any that is
// shared between predictors. (There probably will not
// be any modifications here)
SaturatingCounter* sat_count;
Gshare* g_share;

void PredictorInit() {
    sat_count = new SaturatingCounter(default_counter_state);
    g_share = new Gshare(default_counter_state, 10, 0);
    runs = 0;
}

// This function is called before EVERY run
// It is used to reset predictors and change configurations
void PredictorReset() {

    // Predictor Specific Setup
    if (runs == TWO_BIT_PREDICTOR_)
        printf("Predictor: 2-bit saturating-counter\n");
    else if (runs == GSELECT_PREDICTOR_)
        printf("Predictor: gseletct\n");
    else if (runs == GSHARE_PREDICTOR_)
        printf("Predictor: gshare\n");

    // Branch History Register Resets
    brh_fetch = 0;
    brh_retire = 0;
}

void PredictorRunACycle() {
    // Stores info about what uops are being processed at each pipeline stage
    const cbp3_cycle_activity_t *cycle_info = get_cycle_info();

    /*
    This `for` loop handles all instructions that come in during the
    fetch stage of the processor. During the fetch stage the branch
    predictor makes a prediction (taken or not-taken), but doesn't
    actually know if the branch was taken or not, so it can't
    update itself with new information. In the loop below, add
    the logic for predicting whether or not the branch should be
    taken according to the current State of the predictor. 
     */
    for (int i = 0; i < cycle_info->num_fetch; i++) {
        uint32_t fe_ptr = cycle_info->fetch_q[i];
        const cbp3_uop_dynamic_t *uop = &fetch_entry(fe_ptr)->uop;

        if (!(uop->type & IS_BR_CONDITIONAL)) continue;
        if (runs == TWO_BIT_PREDICTOR_) {
            // -- PLACE YOUR TWO BIT SATURATING COUNTER PREDICTION CODE BELOW
            // (only put predictions in this section, updating states happens
            // below)

            // Set `gpred` based off whether or not a branch should be taken
            bool gpred = sat_count->should_take(); 

            assert(report_pred(fe_ptr, false, gpred));

        } else if (runs == GSELECT_PREDICTOR_) {
            // -- PLACE YOUR GSELECT PREDICTION CODE BELOW
            // (only put predictions in this section, updating states happens
            // below)
            
            // Set `gpred` based off whether or not a branch should be taken
            bool gpred = g_share->should_take(uop->pc); 

            assert(report_pred(fe_ptr, false, gpred));

        } else if (runs == GSHARE_PREDICTOR_) {
            // -- PLACE YOUR GSHARE PREDICTION CODE BELOW
            // (only put predictions in this section, updating states happens
            // below)

            // Set `gpred` based off whether or not a branch should be taken
            bool gpred = true; 

            assert(report_pred(fe_ptr, false, gpred));

        }

        // -- UPDATE THE `brh_fetch` branch history register here. See "hints" in
        // the assignment description for more information on this.
    }

    /*
    This loop handles all instsructions during the retire stage of the
    pipeline. At this stage it is known if the branch was actually taken
    or not (accessible by uop->br_taken variable) so the State of each
    of the predictors can be updated to reflect the result of the branch
    instruction.
    */
    for (int i = 0; i < cycle_info->num_retire; i++) {
        uint32_t rob_ptr = cycle_info->retire_q[i];
        const cbp3_uop_dynamic_t *uop = &rob_entry(rob_ptr)->uop;

        if(!(uop->type & IS_BR_CONDITIONAL)) continue;

        if (runs == TWO_BIT_PREDICTOR_) {
            // -- UPDATE THE STATE OF THE TWO BIT SATURATING COUNTER HERE
            sat_count->taken(uop->br_taken);
        } else if (runs == GSELECT_PREDICTOR_) {
            // -- UPDATE THE STATE OF THE GSELECT HERE
            g_share->taken(uop->br_taken, uop->pc);
        } else if (runs == GSHARE_PREDICTOR_) {
            // -- UPDATE THE STATE OF THE GSHARE HERE
        }

        // -- UPDATE THE `brh_retire` branch history register here. See "hints" in
        // the assignment description for more information on this.
    }
}

void PredictorRunEnd() {
    runs ++;
    if (runs < 3) // set rewind_marked to indicate that we want more runs
        rewind_marked = true;
}

void PredictorExit() {
    delete sat_count;
    delete g_share;
}
