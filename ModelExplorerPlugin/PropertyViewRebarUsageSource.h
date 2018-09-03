#pragma once

#include "IPropertyViewSourceObject.h"

class PropertyViewRebarUsageSource : public IPropertyViewSourceObject
{
public:
  PropertyViewRebarUsageSource(
    Renga::IApplicationPtr pApplication,
    Renga::IRebarUsagePtr pRebarUsage);

  IObjectPropertyViewBuilder* createPropertyViewBuilder(PropertyManagers* pPropertyManagers) override;

  Renga::IPropertyPtr getUserDefinedProperty(GUID propertyId) override;

private:
  Renga::IApplicationPtr m_pApplication;
  Renga::IRebarUsagePtr m_pRebarUsage;
};