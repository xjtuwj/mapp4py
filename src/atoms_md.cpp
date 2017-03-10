#include "atoms_md.h"
/*--------------------------------------------
 
 --------------------------------------------*/
AtomsMD::AtomsMD(MPI_Comm& world):
Atoms(world)
{
    elem=new Vec<elem_type>(this,1,"elem");
    x_d=new Vec<type0>(this,__dim__,"x_d");
    x_d->empty(0.0);
}
/*--------------------------------------------
 
 --------------------------------------------*/
AtomsMD::~AtomsMD()
{
    delete x_d;
    delete elem;
}
/*--------------------------------------------
 
 --------------------------------------------*/
#include "elements.h"
AtomsMD& AtomsMD::operator=(const Atoms& r)
{
    elements=r.elements;
    comm=r.comm;
    natms_lcl=r.natms_lcl;
    natms_ph=r.natms_ph;
    natms=r.natms;
    step=r.step;
    
    max_cut=r.max_cut;
    kB=r.kB;
    h=r.h;
    
    vol=r.vol;
    for(int i=0;i<__dim__;i++)
    {
        depth_inv[i]=r.depth_inv[i];
        for(int j=0;j<__dim__;j++)
        {
            H[i][j]=r.H[i][j];
            B[i][j]=r.B[i][j];
        }
    }
    
    for(int i=0;i<nvecs;i++)
        if(!vecs[i]->is_empty())
            vecs[i]->resize(natms_lcl);
    memcpy(x->begin(),r.x->begin(),natms_lcl*__dim__*sizeof(type0));
    memcpy(id->begin(),r.id->begin(),natms_lcl*sizeof(unsigned int));
    return* this;
}
/*--------------------------------------------
 
 --------------------------------------------*/
#include "random.h"
#include "elements.h"
#include "xmath.h"
void AtomsMD::create_T(type0 T,int seed)
{
    x_d->fill();
    Random rand(seed+comm_rank);
    type0* __x_d=x_d->begin();
    type0* m=elements.masses;
    elem_type* __elem=elem->begin();
    type0 fac;
    if(!dof->is_empty())
    {
        bool* __dof=dof->begin();
        for(int i=0;i<natms_lcl;i++)
        {
            fac=sqrt(kB*T/m[*__elem]);
            Algebra::Do<__dim__>::func([&fac,&rand,&__x_d,__dof,this](const int i){if(__dof[i]) __x_d[i]=rand.gaussian()*fac;});
            __x_d+=__dim__;
            __dof+=__dim__;
            ++__elem;
        }
    }
    else
    {
        for(int i=0;i<natms_lcl;i++)
        {
            fac=sqrt(kB*T/m[*__elem]);
            Algebra::Do<__dim__>::func([&fac,&rand,&__x_d,this](const int i){__x_d[i]=rand.gaussian()*fac;});
            __x_d+=__dim__;
            ++__elem;
        }
    }
}
/*--------------------------------------------
 
 --------------------------------------------*/
/*
void AtomsMD::DO(PyFunctionObject* op)
{
    
    PyCodeObject* co=(PyCodeObject *)PyFunction_GET_CODE(op);
    PyObject* co_varnames=co->co_varnames;
    size_t co_nvars=PyTuple_Size(co_varnames);
    
    constexpr int atoms_nkwds=5;
    enum{__id,__elem,__x,__x_d,__dof};
    const char* atoms_kwds[atoms_nkwds]={[__id]="id",[__elem]="elem",[__x]="x",[__x_d]="x_d",[__dof]="dof"};
    vec* atoms_vecs[atoms_nkwds]={[__id]=id,[__elem]=elem,[__x]=x,[__x_d]=x_d,[__dof]=dof};
    bool found[atoms_nkwds]={[0 ... atoms_nkwds-1]=false};
    
    
    for(size_t i=0;i<co_nvars;i++)
    {
        char* var_name=PyString_AsString(PyTuple_GetItem(co_varnames,i));
        bool var_found=false;
        int j=0;
        for(;j<atoms_nkwds && !var_found;j++)
        if(strcmp(var_name,atoms_kwds[j])==0) var_found=true;
        if(!var_found)
        throw std::string("unknown parameter: ")+std::string(var_name);
        found[j-1]=true;
    }
    
    
    if(found[__dof] && !dof)
    {
        dof=new Vec<bool>(this,__dim__,"dof");
        bool* ___dof=dof->begin();
        for(int i=0;i<natms_lcl*__dim__;i++) ___dof[i]=true;
    }
    if(found[__x_d] && !x_d)
    {
        x_d=new Vec<type0>(this,__dim__,"x_d");
        type0* ___x_d=x_d->begin();
        for(int i=0;i<natms_lcl*__dim__;i++) ___x_d[i]=0.0;
    }
    
    std::remove_pointer<npy_intp>::type npy__dim__=static_cast<std::remove_pointer<npy_intp>::type>(__dim__);
    std::remove_pointer<npy_intp>::type npy__1=static_cast<std::remove_pointer<npy_intp>::type>(1);
    PyObject* Py_x=PyArray_SimpleNewFromData(1,&npy__dim__,cpp_type2type_num<type0>::type_num(),x->begin());

    
}*/
/*------------------------------------------------------------------------------------------------------------------------------------
 
 ------------------------------------------------------------------------------------------------------------------------------------*/
#include "ff_styles.h"
#include "import_styles.h"
PyObject* AtomsMD::__new__(PyTypeObject* type,PyObject* args,PyObject* kwds)
{
    Object* __self=reinterpret_cast<Object*>(type->tp_alloc(type,0));
    PyObject* self=reinterpret_cast<PyObject*>(__self);
    return self;
}
/*--------------------------------------------
 
 --------------------------------------------*/
int AtomsMD::__init__(PyObject* self,PyObject* args,PyObject* kwds)
{
    FuncAPI<> func("__init__");
    if(func(args,kwds)==-1) return -1;
    Object* __self=reinterpret_cast<Object*>(self);
    __self->atoms=NULL;
    __self->ff=NULL;
    return 0;
}
/*--------------------------------------------
 
 --------------------------------------------*/
PyObject* AtomsMD::__alloc__(PyTypeObject* type,Py_ssize_t)
{
    Object* __self=new Object;
    __self->ob_type=type;
    __self->ob_refcnt=1;
    __self->atoms=NULL;
    __self->ff=NULL;
    return reinterpret_cast<PyObject*>(__self);
}
/*--------------------------------------------
 
 --------------------------------------------*/
void AtomsMD::__dealloc__(PyObject* self)
{
    Object* __self=reinterpret_cast<Object*>(self);
    delete __self->ff;
    delete __self->atoms;
    delete __self;
}
/*--------------------------------------------*/
PyTypeObject AtomsMD::TypeObject ={PyObject_HEAD_INIT(NULL)};
/*--------------------------------------------*/
void AtomsMD::setup_tp()
{
    TypeObject.tp_name="mapp.md.atoms";
    
    TypeObject.tp_flags=Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    TypeObject.tp_basicsize=sizeof(Object);
    
    TypeObject.tp_new=__new__;
    TypeObject.tp_init=__init__;
    TypeObject.tp_alloc=__alloc__;
    TypeObject.tp_dealloc=__dealloc__;
    
    setup_tp_getset();
    TypeObject.tp_getset=getset;
    setup_tp_methods();
    TypeObject.tp_methods=methods;
    
    TypeObject.tp_doc=R"---(
    atoms()
    
    Container class for system configuration and force field.

    
    )---";
}
/*--------------------------------------------*/
PyGetSetDef AtomsMD::getset[]={[0 ... 12]={NULL,NULL,NULL,NULL,NULL}};
/*--------------------------------------------*/
void AtomsMD::setup_tp_getset()
{
    getset_step(getset[0]);
    getset_h(getset[1]);
    getset_kB(getset[2]);
    getset_H(getset[3]);
    getset_B(getset[4]);
    getset_vol(getset[5]);
    getset_elems(getset[6]);
    getset_skin(getset[7]);
    getset_comm_rank(getset[8]);
    getset_comm_size(getset[9]);
    getset_comm_coords(getset[10]);
    getset_comm_dims(getset[11]);
}
/*--------------------------------------------*/
PyMethodDef AtomsMD::methods[]={[0 ... 8]={NULL,NULL,0,NULL}};
/*--------------------------------------------*/
void AtomsMD::setup_tp_methods()
{
    ml_strain(methods[0]);
    ml_create_temp(methods[1]);
    ForceFieldLJ::ml_new(methods[2]);
    ForceFieldEAM::ml_new(methods[3],methods[4],methods[5]);
    ForceFieldFS::ml_new(methods[6]);
    ReadCFGMD::ml_import(methods[7]);
}
/*--------------------------------------------
 
 --------------------------------------------*/
void AtomsMD::ml_create_temp(PyMethodDef& tp_method)
{
    tp_method.ml_flags=METH_VARARGS | METH_KEYWORDS;
    tp_method.ml_name="create_temp";
    tp_method.ml_doc="I will add doc here";
    
    tp_method.ml_meth=(PyCFunction)(PyCFunctionWithKeywords)
    [](PyObject* self,PyObject* args,PyObject* kwds)->PyObject*
    {
        FuncAPI<type0,int> f("create_temp",{"temp","seed"});
        f.logics<0>()[0]=VLogics("gt",0.0);
        f.logics<1>()[0]=VLogics("gt",0);
        if(f(args,kwds)) return NULL;
        
        AtomsMD::Object* __self=reinterpret_cast<AtomsMD::Object*>(self);
        __self->atoms->create_T(f.val<0>(),f.val<1>());
        Py_RETURN_NONE;
    };
}

    
    
    
    
    
