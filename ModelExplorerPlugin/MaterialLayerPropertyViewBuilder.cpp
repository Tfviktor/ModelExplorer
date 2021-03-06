#include "stdafx.h"

#include "MaterialLayerPropertyViewBuilder.h"

#include <Renga/QuantityIds.h>

MaterialLayerPropertyViewBuilder::MaterialLayerPropertyViewBuilder(const PropertyManagers* pPropertyManagers,
                                                                   Renga::IApplicationPtr pApplication,
                                                                   Renga::IMaterialLayerPtr pMaterialLayer,
                                                                   Renga::ILayerPtr pLayer) :
  m_pPropertyManagers(pPropertyManagers),
  m_pApplication(pApplication),
  m_pMaterialLayer(pMaterialLayer),
  m_pLayer(pLayer)
{
}

void MaterialLayerPropertyViewBuilder::createIntegratedParameters(PropertyList& propertyList)
{
  auto pMaterial = m_pMaterialLayer->GetMaterial();
  QString materialName = pMaterial != nullptr ? QString::fromWCharArray(pMaterial->Name) : QApplication::translate("me_materialLayer", "noMaterial");
  auto& mngr = m_pPropertyManagers->m_default;
  mngr.addValue(propertyList, QApplication::translate("me_materialLayer", "material"), materialName);
}

void MaterialLayerPropertyViewBuilder::createQuantities(PropertyList& propertyList)
{
  using namespace Renga;

  auto pQuantities = m_pLayer->GetQuantities();

  auto& mngr = m_pPropertyManagers->m_default;
  mngr.addValue(propertyList, QApplication::translate("me_materialLayer", "nominalThickness"), pQuantities->Get(QuantityIds::NominalThickness));
  mngr.addValue(propertyList, QApplication::translate("me_materialLayer", "nominalLength"), pQuantities->Get(QuantityIds::NominalLength));
  mngr.addValue(propertyList, QApplication::translate("me_materialLayer", "nominalHeight"), pQuantities->Get(QuantityIds::NominalHeight));
  mngr.addValue(propertyList, QApplication::translate("me_materialLayer", "netVolume"), pQuantities->Get(QuantityIds::NetVolume));
  mngr.addValue(propertyList, QApplication::translate("me_materialLayer", "netMass"), pQuantities->Get(QuantityIds::NetMass));
  mngr.addValue(propertyList, QApplication::translate("me_materialLayer", "netArea"), pQuantities->Get(QuantityIds::NetArea));
  mngr.addValue(propertyList, QApplication::translate("me_materialLayer", "netFootprintArea"), pQuantities->Get(QuantityIds::NetFootprintArea));
  mngr.addValue(propertyList, QApplication::translate("me_materialLayer", "netSideArea"), pQuantities->Get(QuantityIds::NetSideArea));
  mngr.addValue(propertyList, QApplication::translate("me_materialLayer", "totalSurfaceArea"), pQuantities->Get(QuantityIds::TotalSurfaceArea));
}
