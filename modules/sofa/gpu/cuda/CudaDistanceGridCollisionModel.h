#ifndef SOFA_GPU_CUDA_CUDADISTANCEGRIDCOLLISIONMODEL_H
#define SOFA_GPU_CUDA_CUDADISTANCEGRIDCOLLISIONMODEL_H

#include "CudaTypes.h"

#include <sofa/core/CollisionModel.h>
#include <sofa/core/VisualModel.h>
#include <sofa/component/MechanicalObject.h>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/helper/io/Mesh.h>
#include <sofa/component/topology/MeshTopology.h>

namespace sofa
{

namespace gpu
{

namespace cuda
{

using namespace sofa::defaulttype;
//using namespace sofa::component::collision;

class CudaDistanceGrid
{
public:
    typedef float Real;
    static Real maxDist() { return (Real)1e10; }
    typedef Vec3f Coord;
    //typedef Vec4f SamplingPoint; ///< 3D coordinates + radius
    typedef CudaVector<Real> VecReal;
    typedef CudaVector<Coord> VecCoord;

    CudaDistanceGrid(int nx, int ny, int nz, Coord pmin, Coord pmax);

protected:
    ~CudaDistanceGrid();

public:

    /// Load a distance grid
    static CudaDistanceGrid* load(const std::string& filename, double scale=1.0, int nx=64, int ny=64, int nz=64, Coord pmin = Coord(), Coord pmax = Coord());

    /// Load or reuse a distance grid
    static CudaDistanceGrid* loadShared(const std::string& filename, double scale=1.0, int nx=64, int ny=64, int nz=64, Coord pmin = Coord(), Coord pmax = Coord());

    /// Add one reference to this grid. Note that loadShared already does this.
    CudaDistanceGrid* addRef();

    /// Release one reference, deleting this grid if this is the last
    bool release();

    /// Save current grid
    bool save(const std::string& filename);

    /// Compute distance field from stored mesh
    void calcDistance();

    /// Compute distance field for a cube of the given half-size.
    /// Also create a mesh of points using np points per axis
    void calcCubeDistance(Real dim=1, int np=5);

    /// Update bbox
    void computeBBox();

    int getNx() const { return nx; }
    int getNy() const { return ny; }
    int getNz() const { return nz; }
    const Coord& getCellWidth() const { return cellWidth; }
    const Coord& getInvCellWidth() const { return invCellWidth; }
    const VecReal& getDists() const { return dists; }
    VecReal& getDists() { return dists; }

    int size() const { return nxnynz; }

    const Coord& getBBMin() const { return bbmin; }
    const Coord& getBBMax() const { return bbmax; }
    void setBBMin(const Coord& val) { bbmin = val; }
    void setBBMax(const Coord& val) { bbmax = val; }
    Coord getBBCorner(int i) const { return Coord((i&1)?bbmax[0]:bbmin[0],(i&2)?bbmax[1]:bbmin[1],(i&4)?bbmax[2]:bbmin[2]); }
    bool inBBox(const Coord& p, Real margin=0.0f) const
    {
        for (int c=0; c<3; ++c)
            if (p[c] < bbmin[c]-margin || p[c] > bbmax[c]+margin) return false;
        return true;
    }

    const Coord& getPMin() const { return pmin; }
    const Coord& getPMax() const { return pmax; }
    Coord getCorner(int i) const { return Coord((i&1)?pmax[0]:pmin[0],(i&2)?pmax[1]:pmin[1],(i&4)?pmax[2]:pmin[2]); }

    bool isCube() const { return cubeDim != 0; }
    Real getCubeDim() const { return cubeDim; }

    bool inGrid(const Coord& p) const
    {
        Coord epsilon = cellWidth*0.1;
        for (int c=0; c<3; ++c)
            if (p[c] < pmin[c]+epsilon[c] || p[c] > pmax[c]-epsilon[c]) return false;
        return true;
    }

    Coord clamp(Coord p) const
    {
        for (int c=0; c<3; ++c)
            if (p[c] < pmin[c]) p[c] = pmin[c];
            else if (p[c] > pmax[c]) p[c] = pmax[c];
        return p;
    }

    int ix(const Coord& p) const
    {
        return rfloor((p[0]-pmin[0])*invCellWidth[0]);
    }

    int iy(const Coord& p) const
    {
        return rfloor((p[1]-pmin[1])*invCellWidth[1]);
    }

    int iz(const Coord& p) const
    {
        return rfloor((p[2]-pmin[2])*invCellWidth[2]);
    }

    int index(const Coord& p, Coord& coefs) const
    {
        coefs[0] = (p[0]-pmin[0])*invCellWidth[0];
        coefs[1] = (p[1]-pmin[1])*invCellWidth[1];
        coefs[2] = (p[2]-pmin[2])*invCellWidth[2];
        int x = rfloor(coefs[0]);
        if (x==-1) x=0; else if (x==nx-1) x=nx-2;
        coefs[0] -= x;
        int y = rfloor(coefs[1]);
        if (y==-1) y=0; else if (y==ny-1) y=ny-2;
        coefs[1] -= y;
        int z = rfloor(coefs[2]);
        if (z==-1) z=0; else if (z==nz-1) z=nz-2;
        coefs[2] -= z;
        return x+nx*(y+ny*(z));
    }

    int index(const Coord& p) const
    {
        Coord coefs;
        return index(p, coefs);
    }

    int index(int x, int y, int z)
    {
        return x+nx*(y+ny*(z));
    }

    Coord coord(int x, int y, int z)
    {
        return pmin+Coord(x*cellWidth[0], y*cellWidth[1], z*cellWidth[2]);
    }

    Real operator[](int index) const { return dists[index]; }
    Real& operator[](int index) { return dists[index]; }

    static Real interp(Real coef, Real a, Real b)
    {
        return a+coef*(b-a);
    }

    Real interp(int index, const Coord& coefs) const
    {
        return interp(coefs[2],interp(coefs[1],interp(coefs[0],dists[index          ],dists[index+1        ]),
                interp(coefs[0],dists[index  +nx     ],dists[index+1+nx     ])),
                interp(coefs[1],interp(coefs[0],dists[index     +nxny],dists[index+1   +nxny]),
                        interp(coefs[0],dists[index  +nx+nxny],dists[index+1+nx+nxny])));
    }

    Real interp(const Coord& p) const
    {
        Coord coefs;
        int i = index(p, coefs);
        return interp(i, coefs);
    }

    Coord grad(int index, const Coord& coefs) const
    {
        // val = dist[0][0][0] * (1-x) * (1-y) * (1-z)
        //     + dist[1][0][0] * (  x) * (1-y) * (1-z)
        //     + dist[0][1][0] * (1-x) * (  y) * (1-z)
        //     + dist[1][1][0] * (  x) * (  y) * (1-z)
        //     + dist[0][0][1] * (1-x) * (1-y) * (  z)
        //     + dist[1][0][1] * (  x) * (1-y) * (  z)
        //     + dist[0][1][1] * (1-x) * (  y) * (  z)
        //     + dist[1][1][1] * (  x) * (  y) * (  z)
        // dval / dx = (dist[1][0][0]-dist[0][0][0]) * (1-y) * (1-z)
        //           + (dist[1][1][0]-dist[0][1][0]) * (  y) * (1-z)
        //           + (dist[1][0][1]-dist[0][0][1]) * (1-y) * (  z)
        //           + (dist[1][1][1]-dist[0][1][1]) * (  y) * (  z)
        const Real dist000 = dists[index          ];
        const Real dist100 = dists[index+1        ];
        const Real dist010 = dists[index  +nx     ];
        const Real dist110 = dists[index+1+nx     ];
        const Real dist001 = dists[index     +nxny];
        const Real dist101 = dists[index+1   +nxny];
        const Real dist011 = dists[index  +nx+nxny];
        const Real dist111 = dists[index+1+nx+nxny];
        return Coord(
                interp(coefs[2],interp(coefs[1],dist100-dist000,dist110-dist010),interp(coefs[1],dist101-dist001,dist111-dist011)), //*invCellWidth[0],
                interp(coefs[2],interp(coefs[0],dist010-dist000,dist110-dist100),interp(coefs[0],dist011-dist001,dist111-dist101)), //*invCellWidth[1],
                interp(coefs[1],interp(coefs[0],dist001-dist000,dist101-dist100),interp(coefs[0],dist011-dist010,dist111-dist110))); //*invCellWidth[2]);
    }

    Coord grad(const Coord& p) const
    {
        Coord coefs;
        int i = index(p, coefs);
        return grad(i, coefs);
    }

    Real eval(const Coord& x) const
    {
        Real d;
        if (inGrid(x))
        {
            d = interp(x);
        }
        else
        {
            Coord xclamp = clamp(x);
            d = interp(xclamp);
            d = rsqrt((x-xclamp).norm2() + d*d); // we underestimate the distance
        }
        return d;
    }

    Real quickeval(const Coord& x) const
    {
        Real d;
        if (inGrid(x))
        {
            d = dists[index(x)] - cellWidth[0]; // we underestimate the distance
        }
        else
        {
            Coord xclamp = clamp(x);
            d = dists[index(xclamp)] - cellWidth[0]; // we underestimate the distance
            d = rsqrt((x-xclamp).norm2() + d*d);
        }
        return d;
    }

    Real eval2(const Coord& x) const
    {
        Real d2;
        if (inGrid(x))
        {
            Real d = interp(x);
            d2 = d*d;
        }
        else
        {
            Coord xclamp = clamp(x);
            Real d = interp(xclamp);
            d2 = ((x-xclamp).norm2() + d*d); // we underestimate the distance
        }
        return d2;
    }

    Real quickeval2(const Coord& x) const
    {
        Real d2;
        if (inGrid(x))
        {
            Real d = dists[index(x)] - cellWidth[0]; // we underestimate the distance
            d2 = d*d;
        }
        else
        {
            Coord xclamp = clamp(x);
            Real d = dists[index(xclamp)] - cellWidth[0]; // we underestimate the distance
            d2 = ((x-xclamp).norm2() + d*d);
        }
        return d2;
    }

    //CudaVector<SamplingPoint> meshPts;
    CudaVector<Coord> meshPts;
    sofa::component::topology::MeshTopology::SeqTriangles meshTriangles;
    sofa::component::topology::MeshTopology::SeqQuads meshQuads;

protected:
    int nbRef;
    VecReal dists;
    const int nx,ny,nz, nxny, nxnynz;
    const Coord pmin, pmax;
    const Coord cellWidth, invCellWidth;
    Coord bbmin, bbmax; ///< bounding box of the object, smaller than the grid

    Real cubeDim; ///< Cube dimension (!=0 if this is actually a cube
    //bool updated;

    // Grid shared resources

    struct CudaDistanceGridParams
    {
        std::string filename;
        double scale;
        int nx,ny,nz;
        Coord pmin,pmax;
        bool operator==(const CudaDistanceGridParams& v) const
        {
            if (!(filename == v.filename)) return false;
            if (!(scale    == v.scale   )) return false;
            if (!(nx       == v.nx      )) return false;
            if (!(ny       == v.ny      )) return false;
            if (!(nz       == v.nz      )) return false;
            if (!(pmin[0]  == v.pmin[0] )) return false;
            if (!(pmin[1]  == v.pmin[1] )) return false;
            if (!(pmin[2]  == v.pmin[2] )) return false;
            if (!(pmax[0]  == v.pmax[0] )) return false;
            if (!(pmax[1]  == v.pmax[1] )) return false;
            if (!(pmax[2]  == v.pmax[2] )) return false;
            return true;
        }
        bool operator<(const CudaDistanceGridParams& v) const
        {
            if (filename < v.filename) return true;
            if (filename > v.filename) return false;
            if (scale    < v.scale   ) return true;
            if (scale    > v.scale   ) return false;
            if (nx       < v.nx      ) return false;
            if (nx       > v.nx      ) return true;
            if (ny       < v.ny      ) return false;
            if (ny       > v.ny      ) return true;
            if (nz       < v.nz      ) return false;
            if (nz       > v.nz      ) return true;
            if (pmin[0]  < v.pmin[0] ) return false;
            if (pmin[0]  > v.pmin[0] ) return true;
            if (pmin[1]  < v.pmin[1] ) return false;
            if (pmin[1]  > v.pmin[1] ) return true;
            if (pmin[2]  < v.pmin[2] ) return false;
            if (pmin[2]  > v.pmin[2] ) return true;
            if (pmax[0]  < v.pmax[0] ) return false;
            if (pmax[0]  > v.pmax[0] ) return true;
            if (pmax[1]  < v.pmax[1] ) return false;
            if (pmax[1]  > v.pmax[1] ) return true;
            if (pmax[2]  < v.pmax[2] ) return false;
            if (pmax[2]  > v.pmax[2] ) return true;
            return false;
        }
        bool operator>(const CudaDistanceGridParams& v) const
        {
            if (filename > v.filename) return true;
            if (filename < v.filename) return false;
            if (scale    > v.scale   ) return true;
            if (scale    < v.scale   ) return false;
            if (nx       > v.nx      ) return false;
            if (nx       < v.nx      ) return true;
            if (ny       > v.ny      ) return false;
            if (ny       < v.ny      ) return true;
            if (nz       > v.nz      ) return false;
            if (nz       < v.nz      ) return true;
            if (pmin[0]  > v.pmin[0] ) return false;
            if (pmin[0]  < v.pmin[0] ) return true;
            if (pmin[1]  > v.pmin[1] ) return false;
            if (pmin[1]  < v.pmin[1] ) return true;
            if (pmin[2]  > v.pmin[2] ) return false;
            if (pmin[2]  < v.pmin[2] ) return true;
            if (pmax[0]  > v.pmax[0] ) return false;
            if (pmax[0]  < v.pmax[0] ) return true;
            if (pmax[1]  > v.pmax[1] ) return false;
            if (pmax[1]  < v.pmax[1] ) return true;
            if (pmax[2]  > v.pmax[2] ) return false;
            if (pmax[2]  < v.pmax[2] ) return true;
            return false;
        }
    };
    static std::map<CudaDistanceGridParams, CudaDistanceGrid*>& getShared();

};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CudaRigidDistanceGridCollisionModel;

class CudaRigidDistanceGridCollisionElement : public core::TCollisionElementIterator<CudaRigidDistanceGridCollisionModel>
{
public:
    CudaRigidDistanceGridCollisionElement(CudaRigidDistanceGridCollisionModel* model, int index);

    explicit CudaRigidDistanceGridCollisionElement(core::CollisionElementIterator& i);

    CudaDistanceGrid* getGrid();

    bool isTransformed();
    const Matrix3& getRotation();
    const Vector3& getTranslation();

    void setGrid(CudaDistanceGrid* surf);

    /// @name Previous state data
    /// Used to estimate velocity in case the distance grid itself is dynamic
    /// @{
    CudaDistanceGrid* getPrevGrid();
    const Matrix3& getPrevRotation();
    const Vector3& getPrevTranslation();
    double getPrevDt();
    /// @}

    /// Set new grid and transform, keeping the old state to estimate velocity
    void setNewState(double dt, CudaDistanceGrid* grid, const Matrix3& rotation, const Vector3& translation);
};

class CudaRigidDistanceGridCollisionModel : public core::CollisionModel, public core::VisualModel
{
protected:

    class ElementData
    {
    public:
        Matrix3 rotation;
        Vector3 translation;
        CudaDistanceGrid* grid;

        /// @name Previous state data
        /// Used to estimate velocity in case the distance grid itself is dynamic
        /// @{
        CudaDistanceGrid* prevGrid; ///< Previous grid
        Matrix3 prevRotation; ///< Previous rotation
        Vector3 prevTranslation; ///< Previous translation
        double prevDt; ///< Time difference between previous and current state
        /// @}

        bool isTransformed; ///< True if translation/rotation was set
        ElementData() : grid(NULL), prevGrid(NULL), prevDt(0.0), isTransformed(false) { rotation.identity(); prevRotation.identity(); }
    };

    std::vector<ElementData> elems;
    bool modified;

    // Input data parameters
    DataField< std::string > filename;
    DataField< double > scale;
    DataField< helper::fixed_array<CudaDistanceGrid::Coord,2> > box;
    DataField< int > nx;
    DataField< int > ny;
    DataField< int > nz;
    DataField< std::string > dumpfilename;

    core::componentmodel::behavior::MechanicalState<RigidTypes>* rigid;

    void updateGrid();
public:
    typedef Rigid3Types InDataTypes;
    typedef Vec3Types DataTypes;
    typedef CudaRigidDistanceGridCollisionElement Element;

    DataField< bool > usePoints;

    CudaRigidDistanceGridCollisionModel();

    ~CudaRigidDistanceGridCollisionModel();

    core::componentmodel::behavior::MechanicalState<InDataTypes>* getRigidModel() { return rigid; }
    core::componentmodel::behavior::MechanicalState<InDataTypes>* getMechanicalState() { return rigid; }

    void init();

    CudaDistanceGrid* getGrid(int index=0)
    {
        return elems[index].grid;
    }
    bool isTransformed(int index=0)
    {
        return elems[index].isTransformed;
    }
    const Matrix3& getRotation(int index=0)
    {
        return elems[index].rotation;
    }
    const Vector3& getTranslation(int index=0)
    {
        return elems[index].translation;
    }

    void setGrid(CudaDistanceGrid* surf, int index=0);

    CudaDistanceGrid* getPrevGrid(int index=0)
    {
        return elems[index].prevGrid;
    }
    const Matrix3& getPrevRotation(int index=0)
    {
        return elems[index].prevRotation;
    }
    const Vector3& getPrevTranslation(int index=0)
    {
        return elems[index].prevTranslation;
    }
    double getPrevDt(int index=0)
    {
        return elems[index].prevDt;
    }

    /// Set new grid and transform, keeping the old state to estimate velocity
    void setNewState(int index, double dt, CudaDistanceGrid* grid, const Matrix3& rotation, const Vector3& translation);

    /// @}

    /// Set new grid and transform, keeping the old state to estimate velocity
    void setNewState(double dt, CudaDistanceGrid* grid, const Matrix3& rotation, const Vector3& translation);

    // -- CollisionModel interface

    void resize(int size);

    /// Create or update the bounding volume hierarchy.
    void computeBoundingTree(int maxDepth=0);

    void draw(int index);

    // -- VisualModel interface

    void draw();

    void initTextures() { }

    void update() { }
};

inline CudaRigidDistanceGridCollisionElement::CudaRigidDistanceGridCollisionElement(CudaRigidDistanceGridCollisionModel* model, int index)
    : core::TCollisionElementIterator<CudaRigidDistanceGridCollisionModel>(model, index)
{}

inline CudaRigidDistanceGridCollisionElement::CudaRigidDistanceGridCollisionElement(core::CollisionElementIterator& i)
    : core::TCollisionElementIterator<CudaRigidDistanceGridCollisionModel>(static_cast<CudaRigidDistanceGridCollisionModel*>(i.getCollisionModel()), i.getIndex())
{
}

inline CudaDistanceGrid* CudaRigidDistanceGridCollisionElement::getGrid() { return model->getGrid(index); }
inline void CudaRigidDistanceGridCollisionElement::setGrid(CudaDistanceGrid* surf) { return model->setGrid(surf, index); }

inline bool CudaRigidDistanceGridCollisionElement::isTransformed() { return model->isTransformed(index); }
inline const Matrix3& CudaRigidDistanceGridCollisionElement::getRotation() { return model->getRotation(index); }
inline const Vector3& CudaRigidDistanceGridCollisionElement::getTranslation() { return model->getTranslation(index); }

inline CudaDistanceGrid* CudaRigidDistanceGridCollisionElement::getPrevGrid() { return model->getPrevGrid(index); }
inline const Matrix3& CudaRigidDistanceGridCollisionElement::getPrevRotation() { return model->getPrevRotation(index); }
inline const Vector3& CudaRigidDistanceGridCollisionElement::getPrevTranslation() { return model->getPrevTranslation(index); }
inline double CudaRigidDistanceGridCollisionElement::getPrevDt() { return model->getPrevDt(index); }

inline void CudaRigidDistanceGridCollisionElement::setNewState(double dt, CudaDistanceGrid* grid, const Matrix3& rotation, const Vector3& translation)
{
    return model->setNewState(dt, grid, rotation, translation);
}

} // namespace cuda

} // namespace gpu

} // namespace sofa

#endif
