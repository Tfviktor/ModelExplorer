//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#include "stdafx.h"
#include "ObjectPropertyViewBuilderFactory.h"

#include "LevelPropertyViewBuilder.h"
#include "WallPropertyViewBuilder.h"
#include "ColumnPropertyViewBuilder.h"
#include "FloorPropertyViewBuilder.h"
#include "OpeningPropertyViewBuilder.h"
#include "RoofPropertyViewBuilder.h"
#include "BeamPropertyViewBuilder.h"
#include "StairPropertyViewBuilder.h"
#include "RampPropertyViewBuilder.h"
#include "WindowPropertyViewBuilder.h"
#include "DoorPropertyViewBuilder.h"
#include "RailingPropertyViewBuilder.h"
#include "RoomPropertyViewBuilder.h"
#include "IsolatedFoundationPropertyViewBuilder.h"
#include "WallFoundationPropertyViewBuilder.h"
#include "AssemblyInstancePropertyViewBuilder.h"
#include "ElementPropertyViewBuilder.h"

#include "MaterialLayerPropertyViewBuilder.h"
#include "RebarUsagePropertyViewBuilder.h"
#include "ReinforcementUnitUsagePropertyViewBuilder.h"

#include <Renga/ObjectTypes.h>


ObjectPropertyViewBuilderFactory::ObjectPropertyViewBuilderFactory(const PropertyManagers* pPropertyManagers, Renga::IApplicationPtr pApplication) :
  m_pApplication(pApplication),
  m_pPropertyManagers(pPropertyManagers)
{
}

IObjectPropertyViewBuilder* ObjectPropertyViewBuilderFactory::createBuilderForModelObject(Renga::IModelObjectPtr pSelectedObject)
{
  IObjectPropertyViewBuilder* pResult = nullptr;

  if (!pSelectedObject)
    return pResult;

  const auto type = pSelectedObject->GetObjectType();
  if (type == Renga::ObjectTypes::Level)
    pResult = new LevelPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Wall)
    pResult = new WallPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Column)
    pResult = new ColumnPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Floor)
    pResult = new FloorPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Opening)
    pResult = new OpeningPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Roof)
    pResult = new RoofPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Beam)
    pResult = new BeamPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Stair)
    pResult = new StairPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Ramp)
    pResult = new RampPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Window)
    pResult = new WindowPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Door)
    pResult = new DoorPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Railing)
    pResult = new RailingPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Room)
    pResult = new RoomPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::IsolatedFoundation)
    pResult = new IsolatedFoundationPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::WallFoundation)
    pResult = new WallFoundationPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::AssemblyInstance)
    pResult = new AssemblyInstancePropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);
  else if (type == Renga::ObjectTypes::Element)
    pResult = new ElementPropertyViewBuilder(m_pPropertyManagers, m_pApplication, pSelectedObject);

  return pResult;
}

IObjectPropertyViewBuilder* ObjectPropertyViewBuilderFactory::createBuilderForMaterialLayer(
  Renga::IMaterialLayerPtr pMaterialLayer,
  Renga::ILayerPtr pLayer)
{
  return new MaterialLayerPropertyViewBuilder(
    m_pPropertyManagers, m_pApplication, pMaterialLayer, pLayer);
}

IObjectPropertyViewBuilder* ObjectPropertyViewBuilderFactory::createBuilderForRebarUsage(Renga::IRebarUsagePtr pRebarUsage)
{
  return new RebarUsagePropertyViewBuilder(m_pPropertyManagers, m_pApplication, pRebarUsage);
}

IObjectPropertyViewBuilder* ObjectPropertyViewBuilderFactory::createBuilderForReinforcementUnitUsage(
  Renga::IReinforcementUnitUsagePtr pReinforcementUnitUsage)
{
  return new ReinforcementUnitUsagePropertyViewBuilder(m_pPropertyManagers, m_pApplication, pReinforcementUnitUsage);
}
