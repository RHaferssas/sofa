/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_CONFIGURATIONSETTING_BACKGROUND_H
#define SOFA_COMPONENT_CONFIGURATIONSETTING_BACKGROUND_H
#include "config.h"

#include <sofa/core/objectmodel/ConfigurationSetting.h>
#include <sofa/core/objectmodel/DataFileName.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/helper/types/RGBAColor.h>

namespace sofa
{

namespace component
{

namespace configurationsetting
{

///Class for the configuration of background settings.
class SOFA_BASE_VISUAL_API BackgroundSetting: public core::objectmodel::ConfigurationSetting
{
public:
    SOFA_CLASS(BackgroundSetting,core::objectmodel::ConfigurationSetting);  ///< Sofa macro to define typedef.

protected:
    BackgroundSetting();                                         ///< Default constructor

public:
    Data<sofa::helper::types::RGBAColor> color;                          ///< Color of the Background of the Viewer.
    sofa::core::objectmodel::DataFileName image;                 ///< Image to be used as background of the viewer.

};

}

}

}
#endif
