/*
 * OglViewport.h
 *
 *  Created on: 26 nov. 2009
 *      Author: froy
 */

#ifndef SOFA_COMPONENT_VISUALMODEL_OGLVIEWPORT_H_
#define SOFA_COMPONENT_VISUALMODEL_OGLVIEWPORT_H_

#include <sofa/core/VisualManager.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/helper/gl/FrameBufferObject.h>

namespace sofa
{

namespace component
{

namespace visualmodel
{

class OglViewport : public core::VisualManager
{
public:
    SOFA_CLASS(OglViewport, core::VisualManager);

    Data<defaulttype::Vec<2, unsigned int> > p_screenPosition;
    Data<defaulttype::Vec<2, unsigned int> > p_screenSize;
    Data<defaulttype::Vec3f> p_cameraPosition;
    Data<defaulttype::Quat> p_cameraOrientation;

    helper::gl::FrameBufferObject fbo;


    OglViewport();
    virtual ~OglViewport();

    void init();
    void initVisual();
    void preDrawScene(helper::gl::VisualParameters* vp);
    bool drawScene(helper::gl::VisualParameters* vp);
    void postDrawScene(helper::gl::VisualParameters* /*vp*/);
};

}

}

}

#endif /* SOFA_COMPONENT_VISUALMODEL_OGLVIEWPORT_H_ */
