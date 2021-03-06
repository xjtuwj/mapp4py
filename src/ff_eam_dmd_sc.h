#ifndef __MAPP__ff_eam__dmd_sc__
#define __MAPP__ff_eam__dmd_sc__
#include "ff_dmd.h"
namespace MAPP_NS
{
    template<class> class DMDVec;
    class ForceFieldEAMDMDSC : public ForceFieldDMD
    {
    private:
        size_t nr,nrho;
        type0 dr,drho,dr_inv,drho_inv,rho_max;
        type0(** F_arr)[5];
        type0(*** r_phi_arr)[4];
        type0(*** r_rho_arr)[4];

        
        
        Vec<type0>* E_ptr;
        Vec<type0>* dE_ptr;
        Vec<type0>* rho_ptr;
        DMDVec<type0>* mu_ptr;
        Vec<type0>* cv_ptr;
        Vec<type0>* p_ptr;
        Vec<type0>* c_tilde_ptr;
        
        Vec<type0>* vec0;
        Vec<type0>* vec1;
        Vec<type0>* vec2;
        Vec<type0>* vec3;
        Vec<type0>* fcoef_ptr;
        
        
        /*--------------------------------------------*/
        type0 kbT,beta;
        type0* c_0;
        type0* c_1;
        type0* zeta;
        
        /*--------------------------------------------*/
        type0* rho_phi;
        type0* drho_phi_dr;
        type0* drho_phi_dalpha;
        type0* B_pair;
        int* NN;
        size_t max_pairs;
        
        /*--------------------------------------------*/
        
        type0* xi;
        type0* wi_0;
        type0* wi_1;
        const int N;
        
        /*--------------------------------------------*/
        
        type0 calc_ent(type0);
        void set_temp();


        void calc_pair(type0,type0*,type0*,type0*&,type0*&,type0*&);
        
        /*--------------------------------------------*/
        void calc_Q(type0&,type0&,type0&,type0&,type0&);
        void calc_Q(type0&,type0&,type0&,type0&,type0&,type0&);
        /*--------------------------------------------*/
        
        void sc_loop();
        void sc_loop_var();
        void sc_loop__();
        void sc_loop___();
        void sc_loop____();
        void sc_loop_cluster();
        void sc_loop__Z6_2nd();

        
    protected:
        void force_calc();
        void energy_calc();
        void force_calc_static(){};
        void c_d_calc(){};
        void J(Vec<type0>*,Vec<type0>*){};
    public:
        
        void init_static(){};
        void fin_static(){};
        ForceFieldEAMDMDSC(class AtomsDMD*,
        type0,type0,size_t,size_t,
        type0(***&&)[4],type0(***&&)[4],type0(**&&)[5],
        type0**&&,type0*&&,type0*&&);
    
        ~ForceFieldEAMDMDSC();
        void init();
        void fin();
        
        
        
        type0 prep(VecTens<type0,2>&);
        void J(VecTens<type0,2>&,VecTens<type0,2>&);

        static void ml_new(PyMethodDef&,PyMethodDef&,PyMethodDef&);
    };
}
#endif
 


