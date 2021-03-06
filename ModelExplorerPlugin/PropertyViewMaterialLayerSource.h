#pragma once

#include "IPropertyViewSourceObject.h"

class PropertyViewMaterialLayerSource : public CPropertyViewSourceObject
{
public:
  PropertyViewMaterialLayerSource(Renga::IApplicationPtr pApplication,
                                  Renga::IMaterialLayerPtr pMaterialLayer,
                                  Renga::ILayerPtr pLayer);

  std::unique_ptr<IPropertyViewBuilder> createPropertyViewBuilder(PropertyManagers* pPropertyManagers) override;

private:
  Renga::IApplicationPtr m_pApplication;
  Renga::IMaterialLayerPtr m_pMaterialLayer;
  Renga::ILayerPtr m_pLayer;
};
