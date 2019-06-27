//
// Copyright �Renga Software� LLC, 2016. All rights reserved.
//
// �Renga Software� LLC PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// �Renga Software� LLC  DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//

#pragma once

#include "IPropertyViewBuilder.h"
#include "PropertyManagers.h"

class PropertyViewBuilder : public IPropertyViewBuilder
{
public:
  PropertyViewBuilder(
    const PropertyManagers* pPropertyManagers,
    Renga::IApplicationPtr pApplication,
    Renga::IModelObjectPtr pModelObject);

  virtual void createParametersProperties(PropertyList& propertyList);
  virtual void createParametersPropertiesEx(PropertyList& propertyList);

  virtual void createQuantitiesProperties(PropertyList& propertyList);

  PropertyList createUserAttributesProperties();

  QString getMaterialName(const int& materialId);
  QString getLayeredMaterialName(const int& layeredMaterialId);

  void addValue(PropertyList& propertyList, const QString& name, const int value) const;
  void addValue(PropertyList& propertyList, const QString& name, const double value) const;
  void addValue(PropertyList& propertyList, const QString& name, const QString& value) const;
  void addValue(PropertyList& propertyList, const QString& name, Renga::IQuantityContainerPtr pQuantityContainer, const GUID quantityId) const;
  void addValue(PropertyList& propertyList, const QString& name, Renga::IQuantityPtr pQuantity) const;

protected:
  Renga::IApplicationPtr m_pApplication;
  const PropertyManagers* m_pPropertyManagers;
  Renga::IModelObjectPtr m_pModelObject;
};
